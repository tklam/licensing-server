#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <iostream>
#include <string>
#include <stdexcept>
#include<cryptopp/osrng.h>
#include<cryptopp/integer.h>
#include<cryptopp/nbtheory.h>
#include<cryptopp/dh.h>
#include <cryptopp/dh2.h>
#include<cryptopp/secblock.h>
#include <cryptopp/filters.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/cmac.h>

struct Crypto {
    
    Crypto();
    ~Crypto();
    void decryptXport(const std::string & xport);
    std::string encrypt(const std::string & plainText);
    std::string decrypt(const std::string & cipherText);
    void genXport(const std::string & spubBHex, const std::string & epubBHex);
    void initKeyExchangeParams();
    std::string spubHexStr() const;
    std::string epubHexStr() const;
    std::string xportHexStr() const;
private:    
    CryptoPP::SecByteBlock _spubA;
    CryptoPP::SecByteBlock _epubA;
    CryptoPP::SecByteBlock _sprivA;
    CryptoPP::SecByteBlock _eprivA;
    CryptoPP::SecByteBlock _kek;
    CryptoPP::SecByteBlock _xport;
    CryptoPP::SecByteBlock _cek;

    CryptoPP::AutoSeededRandomPool _rnd;

    CryptoPP::DH2 * _dhA;
    CryptoPP::DH _dh;

    void UnsignedIntegerToByteBlock(const CryptoPP::Integer& x, CryptoPP::SecByteBlock& bytes);
};


#endif
