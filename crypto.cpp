#include "crypto.h"
#include <iostream>
#include <sstream>

using std::string;
using std::ostringstream;
using std::runtime_error;
using CryptoPP::AutoSeededRandomPool;
using CryptoPP::Integer;
using CryptoPP::ModularExponentiation;
using CryptoPP::DH;
using CryptoPP::DH2;
using CryptoPP::SecByteBlock;
using CryptoPP::StringSink;
using CryptoPP::AES;
using CryptoPP::ECB_Mode;
using CryptoPP::CMAC;
using CryptoPP::PrimeAndGenerator;

Crypto::Crypto() {
}

Crypto::~Crypto() {
    delete _dhA;
}

void Crypto::initKeyExchangeParams() {
    // Generated offline using Crypto++.
    // 10224 bit prime.
    Integer p("0xafe25e32abc0aade88c11b102db2d8fe10f6d6c597dc16ae8dfb44b46b2224b303a8d611e90ffbd09f70a6cbf918021e9a53277eacd4b244e1e2d0d4ade94acb505a7e1ce652fe31922070cb95c909d09858cab47167746d677fc0432273135fa7915ddeaea47a0180123bcb986b58638084c0c894166228b32a36ba08e4d0e7");
    Integer q = (p-1)/2;
    Integer g("0x2");

    _dh.AccessGroupParameters().Initialize(p, g);

    if(!_dh.GetGroupParameters().ValidateGroup(_rnd, 3)) {
        throw runtime_error("Failed to validate prime and generator");
    }

    p = _dh.GetGroupParameters().GetModulus();
    q = _dh.GetGroupParameters().GetSubgroupOrder();
    g = _dh.GetGroupParameters().GetGenerator();

    /*
    std::cout << "P: " << std::hex << p << std::endl;
    std::cout << "Q: " << std::hex << q << std::endl;
    std::cout << "G: " << std::hex << g << std::endl;
    */

    // http://groups.google.com/group/sci.crypt/browse_thread/thread/7dc7eeb04a09f0ce
    Integer v = ModularExponentiation(g, q, p);
    if(v != Integer::One())
    {
        std::cerr << "Verification: " << std::hex << v << std::endl;
        throw runtime_error("Failed to verify order of the subgroup");
    }

    //////////////////////////////////////////////////////////////

    _dhA = new CryptoPP::DH2(_dh);

    _sprivA.CleanNew(_dhA->StaticPrivateKeyLength());
    _spubA.CleanNew(_dhA->StaticPublicKeyLength());
    _eprivA.CleanNew(_dhA->EphemeralPrivateKeyLength());
    _epubA.CleanNew(_dhA->EphemeralPublicKeyLength());

    _dhA->GenerateStaticKeyPair(_rnd, _sprivA, _spubA);
    _dhA->GenerateEphemeralKeyPair(_rnd, _eprivA, _epubA);
}

void Crypto::UnsignedIntegerToByteBlock(const Integer& x, SecByteBlock& bytes)
{
    size_t encodedSize = x.MinEncodedSize(Integer::UNSIGNED);
    bytes.resize(encodedSize);
    x.Encode(bytes.BytePtr(), encodedSize, Integer::UNSIGNED);
}

void Crypto::genXport(const std::string & spubBHex, const std::string & epubBHex) {
    Integer spubB_i(spubBHex.c_str());
    Integer epubB_i(epubBHex.c_str());
    
    SecByteBlock spubB, epubB;
    UnsignedIntegerToByteBlock(spubB_i, spubB);
    UnsignedIntegerToByteBlock(epubB_i, epubB);


    SecByteBlock sharedA(_dhA->AgreedValueLength());

    if(!_dhA->Agree(sharedA, _sprivA, _eprivA, spubB, epubB)) {
        throw runtime_error("Failed to reach shared secret (A)");    
    }

    // Take the leftmost 'n' bits for the KEK
    _kek.Assign(sharedA.BytePtr(), AES::DEFAULT_KEYLENGTH);

    // CMAC key follows the 'n' bits used for KEK
    SecByteBlock mack(&sharedA.BytePtr()[AES::DEFAULT_KEYLENGTH], AES::BLOCKSIZE);
    CMAC<AES> cmac(mack.BytePtr(), mack.SizeInBytes());

    // Generate a random CEK
    _cek.CleanNew(AES::DEFAULT_KEYLENGTH);
    _rnd.GenerateBlock(_cek.BytePtr(), _cek.SizeInBytes());    

    // AES in ECB mode is fine - we're encrypting 1 block, so we don't need padding
    ECB_Mode<AES>::Encryption aes;
    aes.SetKey(_kek.BytePtr(), _kek.SizeInBytes());

    // Will hold the encrypted key and cmac
    _xport.CleanNew(AES::BLOCKSIZE /*ENC(CEK)*/ + AES::BLOCKSIZE /*CMAC*/);
    byte* const ptr = _xport.BytePtr();

    // Write the encrypted key in the first 16 bytes, and the CMAC in the second 16 bytes
    // The logical layout of xport:
    //   [    Enc(CEK)    ][  CMAC(Enc(CEK))  ]
    aes.ProcessData(&ptr[0], _cek.BytePtr(), AES::BLOCKSIZE);
    cmac.CalculateTruncatedDigest(&ptr[AES::BLOCKSIZE], AES::BLOCKSIZE, &ptr[0], AES::BLOCKSIZE);

    Integer tmp;
    tmp.Decode(_kek.BytePtr(), _kek.SizeInBytes());
    std::cout << "kek: 0x" << std::hex << tmp << std::endl;
    tmp.Decode(_cek.BytePtr(), _cek.SizeInBytes());
    std::cout << "cek: 0x" << std::hex << tmp << std::endl;
    tmp.Decode(_xport.BytePtr(), _xport.SizeInBytes());
    std::cout << "xport: 0x" << std::hex << tmp << std::endl;

    delete _dhA;
    _dhA = nullptr;
}

std::string Crypto::spubHexStr() const {
    Integer tmp;
    tmp.Decode(_spubA.BytePtr(), _spubA.SizeInBytes());
    ostringstream s;
    s << "0x" << std::hex << tmp;
    return s.str();
}

std::string Crypto::epubHexStr() const {
    Integer tmp;
    tmp.Decode(_epubA.BytePtr(), _epubA.SizeInBytes());
    ostringstream s;
    s << "0x" << std::hex << tmp;
    return s.str();
}

std::string Crypto::xportHexStr() const {
    Integer tmp;
    tmp.Decode(_xport.BytePtr(), _xport.SizeInBytes());
    ostringstream s;
    s << "0x" << std::hex << tmp;
    return s.str();
}

std::string Crypto::encrypt(const std::string & plainText) {
    byte iv[ CryptoPP::AES::BLOCKSIZE ];
    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );

    CryptoPP::AES::Encryption aesEncryption(_cek, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );

    std::string cipherText;

    CryptoPP::StreamTransformationFilter stfEncryptor(
            cbcEncryption, new CryptoPP::StringSink( cipherText ) );
    stfEncryptor.Put(
            reinterpret_cast<const unsigned char*>( 
                plainText.c_str()
            )
            , plainText.length());
    stfEncryptor.MessageEnd();

    return cipherText;
}

std::string Crypto::decrypt(const std::string & cipherText) {
    byte iv[ CryptoPP::AES::BLOCKSIZE ];
    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );

    CryptoPP::AES::Decryption aesDecryption(_cek, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );

    std::string decryptedText;
    CryptoPP::StreamTransformationFilter stfDecryptor(
            cbcDecryption, new CryptoPP::StringSink( decryptedText ) );
    stfDecryptor.Put(
            reinterpret_cast<const unsigned char*>( 
                cipherText.c_str() 
            )
            , cipherText.size() );
    stfDecryptor.MessageEnd();
    return decryptedText;
}

void Crypto::decryptXport(const std::string & xport) {
    SecByteBlock newXport;
    const char * xport_cstr = xport.c_str();
    Integer xport_i(xport_cstr);
    UnsignedIntegerToByteBlock(xport_i, newXport);
    _xport.Assign(newXport);

    Integer tmp;
    tmp.Decode(_xport.BytePtr(), _xport.SizeInBytes());
    std::cout << "new xport: 0x" << std::hex << tmp << std::endl;

    //discard CMAC at the end
    SecByteBlock encryptedCEK(_xport, AES::DEFAULT_KEYLENGTH);

    tmp.Decode(encryptedCEK.BytePtr(), encryptedCEK.SizeInBytes());
    std::cout << "encrypted cek: 0x" << std::hex << tmp << std::endl;

    SecByteBlock decryptedCEK(AES::DEFAULT_KEYLENGTH);
    // AES in ECB mode is fine - we're encrypting 1 block, so we don't need padding
    ECB_Mode<AES>::Decryption aes;
    aes.SetKey(_kek.BytePtr(), _kek.SizeInBytes());
    aes.ProcessData(decryptedCEK.BytePtr(), encryptedCEK.BytePtr(), AES::BLOCKSIZE);

    tmp.Decode(decryptedCEK.BytePtr(), decryptedCEK.SizeInBytes());
    std::cout << "decrypted cek: 0x" << std::hex << tmp << std::endl;
    _cek.Assign(decryptedCEK);
}
