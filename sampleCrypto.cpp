#include <iostream>


#include <string>
#include "crypto.h"
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

using namespace std;

int testDiffieHellman() {   
    try
    {
        Crypto crypto_A, crypto_B;
        crypto_A.initKeyExchangeParams();
        crypto_B.initKeyExchangeParams();

        const std::string crypto_A_spub = crypto_A.spubHexStr();
        const std::string crypto_A_epub = crypto_A.epubHexStr();
        const std::string crypto_B_spub = crypto_B.spubHexStr();
        const std::string crypto_B_epub = crypto_B.epubHexStr();

        std::cout << "crypto_A_spub: " << crypto_A_spub << std::endl;
        std::cout << "crypto_A_epub: " << crypto_A_epub << std::endl;
        std::cout << "crypto_B_spub: " << crypto_B_spub << std::endl;
        std::cout << "crypto_B_epub: " << crypto_B_epub << std::endl;

        crypto_A.genXport(crypto_B_spub, crypto_B_epub);
        crypto_B.genXport(crypto_A_spub, crypto_A_epub);

        // suupose crypto_A sends encrypted CEK to crypto_B
        std::string xport = crypto_A.xportHexStr();
        std::cout << "XPORT: " << xport << std::endl;
        crypto_B.decryptXport(xport);

        // encryption using CEK

        //
        // String and Sink setup
        //
        std::string plaintext = "Now is the time for all good men to come to the aide...";
        std::string ciphertext;
        std::string decryptedtext;

        //
        // Dump Plain Text
        //
        std::cout << "Plain Text (" << plaintext.size() << " bytes)" << std::endl;
        std::cout << plaintext;
        std::cout << std::endl << std::endl;

        //
        // Encrypt
        //
        ciphertext = crypto_B.encrypt(plaintext);

        //
        // Dump Cipher Text
        //
        std::cout << "Cipher Text (" << ciphertext.size() << " bytes)" << std::endl;
        std::cout << ciphertext << std::endl;

        //
        // Decrypt
        //
        decryptedtext = crypto_B.decrypt(ciphertext);

        //
        // Dump Decrypted Text
        //
        std::cout << "Decrypted Text: " << std::endl;
        std::cout << decryptedtext;
        std::cout << std::endl << std::endl;
    }

    catch(const CryptoPP::Exception& e)
    {
        cerr << e.what() << endl;
        return -2;
    }

    catch(const std::exception& e)
    {
        cerr << e.what() << endl;
        return -1;
    }

    return 0;
}

int testDiffieHellman_golden() {   
    try
    {

        //Diffie Hellman key exchange

        AutoSeededRandomPool rnd;
        /*
        Integer p, q, g;
        PrimeAndGenerator pg;

        pg.Generate(1, rnd, 1024, 1023);
        p = pg.Prime();
        q = pg.SubPrime();
        g = pg.Generator();

        */

        // Generated offline using Crypto++.
        // 10224 bit prime.
        Integer p("0xafe25e32abc0aade88c11b102db2d8fe10f6d6c597dc16ae8dfb44b46b2224b303a8d611e90ffbd09f70a6cbf918021e9a53277eacd4b244e1e2d0d4ade94acb505a7e1ce652fe31922070cb95c909d09858cab47167746d677fc0432273135fa7915ddeaea47a0180123bcb986b58638084c0c894166228b32a36ba08e4d0e7");
        Integer q = (p-1)/2;
        Integer g("0x2");

        cout << "P: " << std::hex << p << endl;
        cout << "Q: " << std::hex << q << endl;
        cout << "G: " << std::hex << g << endl;

        // Safe primes are of the form p = 2q + 1, p and q prime.
        // These parameters do not state a maximum security level based
        // on the prime subgroup order. In essence, we get the maximum
        // security level. There is no free lunch: it means more modular
        // mutliplications are performed, which affects performance.

        // For a compare/contrast of meeting a security level, see dh-init.zip.
        // Also see http://www.cryptopp.com/wiki/Diffie-Hellman and
        // http://www.cryptopp.com/wiki/Security_level .

        DH dh;      

        dh.AccessGroupParameters().Initialize(p, g);

        if(!dh.GetGroupParameters().ValidateGroup(rnd, 3))
            throw runtime_error("Failed to validate prime and generator");

        size_t count = 0;

        p = dh.GetGroupParameters().GetModulus();
        q = dh.GetGroupParameters().GetSubgroupOrder();
        g = dh.GetGroupParameters().GetGenerator();

        cout << "P: " << std::hex << p << endl;
        cout << "Q: " << std::hex << q << endl;
        cout << "G: " << std::hex << g << endl;

        // http://groups.google.com/group/sci.crypt/browse_thread/thread/7dc7eeb04a09f0ce
        Integer v = ModularExponentiation(g, q, p);
        if(v != Integer::One())
        {
            cout << "Verification: " << std::hex << v << endl;
            throw runtime_error("Failed to verify order of the subgroup");
        }

        //////////////////////////////////////////////////////////////

        DH2 dhA(dh), dhB(dh);

        SecByteBlock sprivA(dhA.StaticPrivateKeyLength()), spubA(dhA.StaticPublicKeyLength());
        SecByteBlock eprivA(dhA.EphemeralPrivateKeyLength()), epubA(dhA.EphemeralPublicKeyLength());

        SecByteBlock sprivB(dhB.StaticPrivateKeyLength()), spubB(dhB.StaticPublicKeyLength());
        SecByteBlock eprivB(dhB.EphemeralPrivateKeyLength()), epubB(dhB.EphemeralPublicKeyLength());

        dhA.GenerateStaticKeyPair(rnd, sprivA, spubA);
        dhA.GenerateEphemeralKeyPair(rnd, eprivA, epubA);

        dhB.GenerateStaticKeyPair(rnd, sprivB, spubB);      
        dhB.GenerateEphemeralKeyPair(rnd, eprivB, epubB);

        //////////////////////////////////////////////////////////////

        if(dhA.AgreedValueLength() != dhB.AgreedValueLength())
            throw runtime_error("Shared secret size mismatch");

        SecByteBlock sharedA(dhA.AgreedValueLength()), sharedB(dhB.AgreedValueLength());

        if(!dhA.Agree(sharedA, sprivA, eprivA, spubB, epubB))
            throw runtime_error("Failed to reach shared secret (A)");

        if(!dhB.Agree(sharedB, sprivB, eprivB, spubA, epubA))
            throw runtime_error("Failed to reach shared secret (B)");

        count = std::min(dhA.AgreedValueLength(), dhB.AgreedValueLength());
        if(!count || 0 != memcmp(sharedA.BytePtr(), sharedB.BytePtr(), count))
            throw runtime_error("Failed to reach shared secret");

        //////////////////////////////////////////////////////////////

        // Take the leftmost 'n' bits for the KEK
        SecByteBlock kek(sharedA.BytePtr(), AES::DEFAULT_KEYLENGTH);

        // CMAC key follows the 'n' bits used for KEK
        SecByteBlock mack(&sharedA.BytePtr()[AES::DEFAULT_KEYLENGTH], AES::BLOCKSIZE);
        CMAC<AES> cmac(mack.BytePtr(), mack.SizeInBytes());

        // Generate a random CEK
        SecByteBlock cek(AES::DEFAULT_KEYLENGTH);
        rnd.GenerateBlock(cek.BytePtr(), cek.SizeInBytes());    

        // AES in ECB mode is fine - we're encrypting 1 block, so we don't need padding
        ECB_Mode<AES>::Encryption aes;
        aes.SetKey(kek.BytePtr(), kek.SizeInBytes());

        // Will hold the encrypted key and cmac
        SecByteBlock xport(AES::BLOCKSIZE /*ENC(CEK)*/ + AES::BLOCKSIZE /*CMAC*/);
        byte* const ptr = xport.BytePtr();

        // Write the encrypted key in the first 16 bytes, and the CMAC in the second 16 bytes
        // The logical layout of xport:
        //   [    Enc(CEK)    ][  CMAC(Enc(CEK))  ]
        aes.ProcessData(&ptr[0], cek.BytePtr(), AES::BLOCKSIZE);
        cmac.CalculateTruncatedDigest(&ptr[AES::BLOCKSIZE], AES::BLOCKSIZE, &ptr[0], AES::BLOCKSIZE);

        Integer a, b, c;

        a.Decode(kek.BytePtr(), kek.SizeInBytes());
        cout << "Key encryption key: " << std::hex << a << endl;

        b.Decode(cek.BytePtr(), cek.SizeInBytes());
        cout << "Content encryption key: " << std::hex << b << endl;

        c.Decode(xport.BytePtr(), xport.SizeInBytes());
        cout << "Encrypted CEK with CMAC: " << std::hex << c << endl;


        // encryption using CEK



        //
        // String and Sink setup
        //
        std::string plaintext = "Now is the time for all good men to come to the aide...";
        std::string ciphertext;
        std::string decryptedtext;

        //
        // Dump Plain Text
        //
        std::cout << "Plain Text (" << plaintext.size() << " bytes)" << std::endl;
        std::cout << plaintext;
        std::cout << std::endl << std::endl;

        byte iv[ CryptoPP::AES::BLOCKSIZE ];
        memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );

        CryptoPP::AES::Encryption aesEncryption(cek, CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );

        CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext ) );
        stfEncryptor.Put( reinterpret_cast<const unsigned char*>( plaintext.c_str() ), plaintext.length() + 1 );
        stfEncryptor.MessageEnd();

        //
        // Dump Cipher Text
        //
        std::cout << "Cipher Text (" << ciphertext.size() << " bytes)" << std::endl;

        std::cout << ciphertext << std::endl;

        //
        // Decrypt
        //
        CryptoPP::AES::Decryption aesDecryption(cek, CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );

        CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( decryptedtext ) );
        stfDecryptor.Put( reinterpret_cast<const unsigned char*>( ciphertext.c_str() ), ciphertext.size() );
        stfDecryptor.MessageEnd();

        //
        // Dump Decrypted Text
        //
        std::cout << "Decrypted Text: " << std::endl;
        std::cout << decryptedtext;
        std::cout << std::endl << std::endl;
    }

    catch(const CryptoPP::Exception& e)
    {
        cerr << e.what() << endl;
        return -2;
    }

    catch(const std::exception& e)
    {
        cerr << e.what() << endl;
        return -1;
    }

    return 0;
}

int main(int argc, char** argv) {
    std::cout << "////////////////////////////Golden implementaion" << std::endl;
    testDiffieHellman_golden();

    std::cout << "///////////////////////////Test implementaion" << std::endl;
    testDiffieHellman();
    return 0;
}
