#ifndef CRYPTO_H
#define CRYPTO_H

#include "Header.h"
#include "openssl/evp.h"
#include "openssl/pem.h"
#include "openssl/aes.h"
#include "openssl/err.h"
#include "openssl/rand.h"
#include "openssl/md5.h"
#include "openssl/ossl_typ.h"
#include "Base64.h"

#define RSA_KEYLEN 2048
#define AES_KEYLEN 256
#define AES_ROUNDS 6

#define PUB_EXP     3

#define PSUEDO_CLIENT

//#define USE_PBKDF

#define SUCCESS 0
#define FAILURE -1

#define KEY_SERVER_PRI 0
#define KEY_SERVER_PUB 1
#define KEY_CLIENT_PUB 2
#define KEY_AES        3
#define KEY_AES_IV     4

class Crypto {
public:
	Crypto();

	Crypto(unsigned char *remotePubKey, size_t remotePubKeyLen);

	~Crypto();

	int GeneratePairKey();

	int getPriKey(unsigned char **priKey);

	int setPriKey(unsigned char* priKey, size_t priKeyLen);

	int setPriKeyFile(char* filePath);

	int getPubKey(unsigned char **pubKey);

	int setPubKey(const char* pubKey, size_t pubKeyLen);

	int setPubKeyFile(char* filePath);

	int rsaEncrypt(const unsigned char *msg, size_t msgLen, unsigned char **encMsg, unsigned char **ek, size_t *ekl, unsigned char **iv, size_t *ivl);

	int aesEncrypt(const unsigned char *msg, size_t msgLen, unsigned char **encMsg);

	int rsaDecrypt(unsigned char *encMsg, size_t encMsgLen, unsigned char *ek, size_t ekl, unsigned char *iv, size_t ivl, unsigned char **decMsg);

	int aesDecrypt(unsigned char *encMsg, size_t encMsgLen, unsigned char **decMsg);

	int rsaSign(unsigned char *encMsg, size_t encMsgLen, unsigned char **decMsg);

	int rsaVerify(unsigned char *encMsg, size_t encMsgLen, unsigned char **decMsg);

	int writeKeyToFile(FILE *fd, int key);

	int getRemotePubKey(unsigned char **pubKey);

	int setRemotePubKey(unsigned char *pubKey, size_t pubKeyLen);

	int getLocalPubKey(unsigned char **pubKey);

	int getLocalPriKey(unsigned char **priKey);

	int getAESKey(unsigned char **aesKey);

	int setAESKey(unsigned char *aesKey, size_t aesKeyLen);

	int setAESKey(std::string aesKey);

	int getAESIv(unsigned char **aesIv);

	int setAESIv(unsigned char *aesIv, size_t aesIvLen);

	int AESEncrypt(std::string Message);

	int AESDecrypt(std::string Message);

	int RSAEncrypt(std::string Message);

	int RSADecrypt(std::string Message);

	unsigned char *encMsg;
	unsigned char *decMsg;

	std::string strError;
	std::string strEncryptMessage;
	std::string strDecryptMessage;

	string privateKey;
	string publicKey;

	RSA *keyPair;

private:
	static EVP_PKEY *localKeypair;
	EVP_PKEY *remotePubKey;

	EVP_CIPHER_CTX *rsaEncryptCtx;
	EVP_CIPHER_CTX *aesEncryptCtx;

	EVP_CIPHER_CTX *rsaDecryptCtx;
	EVP_CIPHER_CTX *aesDecryptCtx;

	unsigned char *aesKey;
	unsigned char *aesIV;

	int init();
	int genTestClientKey();

	int encMsgLen;
	int decMsgLen;

	unsigned char *ek;
	unsigned char *iv;
	size_t ekl;
	size_t ivl;
};

#endif
