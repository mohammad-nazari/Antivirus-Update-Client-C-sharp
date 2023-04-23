#include "Crypto.h"

using namespace std;

EVP_PKEY* Crypto::localKeypair;

Crypto::Crypto()
{
	localKeypair  = NULL;
	remotePubKey  = NULL;

	this->encMsg = NULL;
	this->decMsg = NULL;

#ifdef PSUEDO_CLIENT
	genTestClientKey();
#endif

	init();
}

Crypto::Crypto(unsigned char *remotePubKey, size_t remotePubKeyLen)
{
	localKeypair        = NULL;
	this->remotePubKey  = NULL;

	this->encMsg = NULL;
	this->decMsg          = NULL;

	setRemotePubKey(remotePubKey, remotePubKeyLen);
	init();
}

Crypto::~Crypto()
{
	EVP_PKEY_free(remotePubKey);

	EVP_CIPHER_CTX_cleanup(rsaEncryptCtx);
	EVP_CIPHER_CTX_cleanup(aesEncryptCtx);

	EVP_CIPHER_CTX_cleanup(rsaDecryptCtx);
	EVP_CIPHER_CTX_cleanup(aesDecryptCtx);

	free(this->rsaEncryptCtx);
	free(this->aesEncryptCtx);

	free(this->rsaDecryptCtx);
	free(this->aesDecryptCtx);

	free(this->aesKey);
	free(this->aesIV);

	free(this->encMsg);
	free(this->decMsg);
}

//************************************
// Method:    GeneratePairKey
// FullName:  Crypto::GeneratePairKey
// Access:    public 
// Returns:   bool
// Qualifier:
// Summary:	  
//************************************

int Crypto::GeneratePairKey()
{
	this->keyPair = RSA_generate_key(RSA_KEYLEN, PUB_EXP, NULL, NULL);
	BIO *pri = BIO_new(BIO_s_mem());
	BIO *pub = BIO_new(BIO_s_mem());

	PEM_write_bio_RSAPrivateKey(pri, this->keyPair, NULL, NULL, 0, NULL, NULL);
	PEM_write_bio_RSAPublicKey(pub, this->keyPair);

	size_t pri_len = BIO_pending(pri);
	size_t pub_len = BIO_pending(pub);

	char *pri_key = (char *) malloc(pri_len + 1);
	char *pub_key = (char *) malloc(pub_len + 1);

	BIO_read(pri, pri_key, (int)pri_len);
	BIO_read(pub, pub_key, (int)pub_len);

	pri_key[pri_len] = '\0';
	pub_key[pub_len] = '\0';

	BIO_free_all(pri);
	BIO_free_all(pub);

	this->privateKey = pri_key;
	this->publicKey = pub_key;

	return SUCCESS;
}

//************************************
// Method:    getPriKey
// FullName:  Crypto::getPriKey
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: unsigned char * * priKey
// Summary:	  
//************************************

int Crypto::getPriKey( unsigned char **priKey )
{

	BIO *bio = BIO_new(BIO_s_mem());
	PEM_write_bio_RSAPrivateKey(bio, this->keyPair, NULL, NULL, 0, NULL, NULL);

	int priKeyLen = BIO_pending(bio);
	*priKey = (unsigned char*) malloc(priKeyLen);
	if (priKey == NULL) return FAILURE;

	BIO_read(bio, *priKey, priKeyLen);

	// Insert the NUL terminator
	(*priKey)[priKeyLen - 1] = '\0';

	BIO_free_all(bio);

	return priKeyLen;
}

//************************************
// Method:    setPriKey
// FullName:  Crypto::setPriKey
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: unsigned char * priKey
// Parameter: size_t priKeyLen
// Summary:	  
//************************************

int Crypto::setPriKey(unsigned char* priKey, size_t priKeyLen)
{
	//BIO *bio = BIO_new(BIO_f_base64());
	BIO *bio = BIO_new(BIO_s_mem());
	if (BIO_write(bio, priKey, (int)priKeyLen) != (int) priKeyLen)
	{
		return FAILURE;
	}

	PEM_read_bio_RSAPrivateKey(bio, &this->keyPair, NULL, NULL);

	BIO_free_all(bio);

	return SUCCESS;
}

//************************************
// Method:    setPriKeyFile
// FullName:  Crypto::setPriKeyFile
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: char * filePath
// Summary:	  
//************************************

int Crypto::setPriKeyFile(char* filePath)
{
	BIO *bio = BIO_new_file(filePath, "r");
	if (!bio)
	{
		return FAILURE;
	}

	PEM_read_bio_RSAPrivateKey(bio, &(this->keyPair), NULL, NULL);

	BIO_free_all(bio);

	return SUCCESS;
}

//************************************
// Method:    getPubKey
// FullName:  Crypto::getPubKey
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: unsigned char * * pubKey
// Summary:	  
//************************************

int Crypto::getPubKey(unsigned char **pubKey)
{

	BIO *bio = BIO_new(BIO_s_mem());
	PEM_write_bio_RSA_PUBKEY(bio, this->keyPair);

	int pubKeyLen = BIO_pending(bio);
	*pubKey = (unsigned char*) malloc(pubKeyLen);
	if (pubKey == NULL) return FAILURE;

	BIO_read(bio, *pubKey, pubKeyLen);

	// Insert the NUL terminator
	(*pubKey)[pubKeyLen - 1] = '\0';

	BIO_free_all(bio);

	return pubKeyLen;
}

//************************************
// Method:    setPubKey
// FullName:  Crypto::setPubKey
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: const char * pubKey
// Parameter: size_t pubKeyLen
// Summary:	  
//************************************

int Crypto::setPubKey(const char* pubKey, size_t pubKeyLen)
{
	//BIO *bio = BIO_new(BIO_f_base64());
	BIO *bio = BIO_new(BIO_s_mem());
	if (BIO_write(bio, pubKey, (int)pubKeyLen) != (int) pubKeyLen)
	{
		return FAILURE;
	}
	if (this->keyPair == NULL) return FAILURE;

	PEM_read_bio_RSA_PUBKEY(bio, &this->keyPair, NULL, NULL);
	if (this->keyPair)
	{
		BIO_free_all(bio);
		return SUCCESS;
	}
	BIO_free_all(bio);

	return FAILURE;
}

//************************************
// Method:    setPubKeyFile
// FullName:  Crypto::setPubKeyFile
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: char * filePath
// Summary:	  
//************************************

int Crypto::setPubKeyFile(char* filePath)
{
	//BIO *bio = BIO_new(BIO_f_base64());

	BIO *bio = BIO_new_file(filePath, "rb");
	if (!bio)
	{
		return FAILURE;
	}

	PEM_read_bio_RSA_PUBKEY(bio, &this->keyPair, NULL, NULL);

	BIO_free_all(bio);


	return SUCCESS;
}

int Crypto::rsaEncrypt(const unsigned char *msg, size_t msgLen, unsigned char **encMsg, unsigned char **ek, size_t *ekl, unsigned char **iv, size_t *ivl)
{
	size_t encMsgLen = 0;
	size_t blockLen  = 0;

	*ek = (unsigned char*) malloc(EVP_PKEY_size(remotePubKey));
	*iv = (unsigned char*) malloc(EVP_MAX_IV_LENGTH);
	if (*ek == NULL || *iv == NULL) return FAILURE;
	*ivl = EVP_MAX_IV_LENGTH;

	*encMsg = (unsigned char*) malloc(msgLen + EVP_MAX_IV_LENGTH);
	if (encMsg == NULL) return FAILURE;

	if (!EVP_SealInit(rsaEncryptCtx, EVP_aes_256_cbc(), ek, (int*) ekl, *iv, &remotePubKey, 1))
	{
		return FAILURE;
	}

	if (!EVP_SealUpdate(rsaEncryptCtx, *encMsg + encMsgLen, (int*) &blockLen, (const unsigned char*) msg, (int) msgLen))
	{
		return FAILURE;
	}
	encMsgLen += blockLen;

	if (!EVP_SealFinal(rsaEncryptCtx, *encMsg + encMsgLen, (int*) &blockLen))
	{
		return FAILURE;
	}
	encMsgLen += blockLen;

	EVP_CIPHER_CTX_cleanup(rsaEncryptCtx);

	return (int) encMsgLen;
}

int Crypto::rsaDecrypt(unsigned char *encMsg, size_t encMsgLen, unsigned char *ek, size_t ekl, unsigned char *iv, size_t ivl, unsigned char **decMsg)
{
	size_t decLen   = 0;
	size_t blockLen = 0;
	EVP_PKEY *key;

	*decMsg = (unsigned char*) malloc(encMsgLen + ivl);
	if (decMsg == NULL) return FAILURE;

#ifdef PSUEDO_CLIENT
	key = remotePubKey;
#else
	key = localKeypair;
#endif

	if (!EVP_OpenInit(rsaDecryptCtx, EVP_aes_256_cbc(), ek, (int)ekl, iv, key))
	{
		return FAILURE;
	}

	if (!EVP_OpenUpdate(rsaDecryptCtx, (unsigned char*) *decMsg + decLen, (int*) &blockLen, encMsg, (int) encMsgLen))
	{
		return FAILURE;
	}
	decLen += blockLen;

	if (!EVP_OpenFinal(rsaDecryptCtx, (unsigned char*) *decMsg + decLen, (int*) &blockLen))
	{
		return FAILURE;
	}
	decLen += blockLen;

	(*decMsg)[decLen] = '\0';

	EVP_CIPHER_CTX_cleanup(rsaDecryptCtx);

	return (int) decLen;
}

int Crypto::rsaSign(unsigned char *encMsg, size_t encMsgLen, unsigned char **decMsg)
{
	//    EVP_MD_CTX *mdctx = NULL;
	//    int slen, ret = 0;
	//
	//    unsigned char *sig = NULL;
	//
	//    /* Create the Message Digest Context */
	//    if (!(mdctx = EVP_MD_CTX_create()))
	//	{
	//	return FAILURE;
	//    }
	//
	//    /* Initialise the DigestSign operation - SHA-256 has been selected as the message digest function in this example */
	//    if (1 != EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, this->aesKey))
	//    {
	//	return FAILURE;
	//    }
	//
	//    /* Call update with the message */
	//    if (1 != EVP_DigestSignUpdate(mdctx, encMsg, encMsgLen))
	//    {
	//	return FAILURE;
	//    }
	//
	//    /* Finalise the DigestSign operation */
	//    /* First call EVP_DigestSignFinal with a NULL sig parameter to obtain the length of the
	//     * signature. Length is returned in slen */
	//    if (1 != EVP_DigestSignFinal(mdctx, NULL, slen))
	//    {
	//	return FAILURE;
	//    }
	//    /* Allocate memory for the signature based on size in slen */
	//    if (!(*sig = OPENSSL_malloc(sizeof (unsigned char) * (*slen))))
	//    {
	//	return FAILURE;
	//    }
	//    /* Obtain the signature */
	//    if (1 != EVP_DigestSignFinal(mdctx, *sig, slen))
	//    {
	//	return FAILURE;
	//    }
	//
	//    /* Success */
	//    ret = 1;
	//
	//    /* Clean up */
	//    if (*sig && !ret) OPENSSL_free(*sig);
	//    if (mdctx) EVP_MD_CTX_destroy(mdctx);

	return SUCCESS;
}

int Crypto::rsaVerify(unsigned char *encMsg, size_t encMsgLen, unsigned char **decMsg)
{
	//    EVP_MD_CTX *mdctx = NULL;
	//    int slen, ret = 0;
	//
	//    unsigned char *sig = NULL;
	//
	//    /* Create the Message Digest Context */
	//    if (!(mdctx = EVP_MD_CTX_create()))
	//	{
	//	return FAILURE;
	//    }
	//
	//    /* Initialise the DigestSign operation - SHA-256 has been selected as the message digest function in this example */
	//    if (1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, this->aesKey))
	//    {
	//	return FAILURE;
	//    }
	//
	//    /* Call update with the message */
	//    if (1 != EVP_DigestSignUpdate(mdctx, encMsg, encMsgLen))
	//    {
	//	return FAILURE;
	//    }
	//
	//    /* Finalise the DigestSign operation */
	//    /* First call EVP_DigestSignFinal with a NULL sig parameter to obtain the length of the
	//     * signature. Length is returned in slen */
	//    if (1 != EVP_DigestVerifyFinal(mdctx, NULL, slen))
	//    {
	//	return FAILURE;
	//    }
	//    /* Allocate memory for the signature based on size in slen */
	//    if (!(*sig = OPENSSL_malloc(sizeof (unsigned char) * (*slen))))
	//    {
	//	return FAILURE;
	//    }
	//    /* Obtain the signature */
	//    if (1 != EVP_DigestVerifyFinal(mdctx, *sig, slen))
	//    {
	//	return FAILURE;
	//    }
	//
	//    /* Success */
	//    ret = 1;
	//
	//    /* Clean up */
	//    if (*sig && !ret) OPENSSL_free(*sig);
	//    if (mdctx) EVP_MD_CTX_destroy(mdctx);

	return SUCCESS;
}

int Crypto::aesEncrypt(const unsigned char *msg, size_t msgLen, unsigned char **encMsg)
{
	size_t blockLen  = 0;
	size_t encMsgLen = 0;

	*encMsg = (unsigned char*) malloc(msgLen + AES_BLOCK_SIZE);
	if (encMsg == NULL) return FAILURE;

	if ((AES_KEYLEN / 8) == 32)
	{

		if (!EVP_EncryptInit_ex(aesEncryptCtx, EVP_aes_256_cbc(), NULL, this->aesKey, NULL))
		{
			return FAILURE;
		}
	}
	else
	{
		if (!EVP_EncryptInit_ex(aesEncryptCtx, EVP_aes_128_cbc(), NULL, this->aesKey, NULL))
		{
			return FAILURE;
		}
	}


	if (!EVP_EncryptUpdate(aesEncryptCtx, *encMsg, (int*) &blockLen, (unsigned char*) msg, (int)msgLen))
	{
		return FAILURE;
	}
	encMsgLen += blockLen;

	if (!EVP_EncryptFinal_ex(aesEncryptCtx, *encMsg + encMsgLen, (int*) &blockLen))
	{
		return FAILURE;
	}

	EVP_CIPHER_CTX_cleanup(aesEncryptCtx);

	return ((int)encMsgLen + (int)blockLen);
}

int Crypto::aesDecrypt(unsigned char *encMsg, size_t encMsgLen, unsigned char **decMsg)
{
	size_t decLen   = 0;
	size_t blockLen = 0;

	*decMsg = (unsigned char*) malloc(encMsgLen);
	if (*decMsg == NULL) return FAILURE;

	if ((AES_KEYLEN / 8) == 32)
	{
		if (!EVP_DecryptInit_ex(aesDecryptCtx, EVP_aes_256_cbc(), NULL, this->aesKey, NULL))
		{
			return FAILURE;
		}
	}
	else
	{
		if (!EVP_DecryptInit_ex(aesDecryptCtx, EVP_aes_128_cbc(), NULL, this->aesKey, NULL))
		{
			return FAILURE;
		}
	}


	if (!EVP_DecryptUpdate(aesDecryptCtx, (unsigned char*) *decMsg, (int*) &blockLen, encMsg, (int) encMsgLen))
	{
		return FAILURE;
	}
	decLen += blockLen;

	if (!EVP_DecryptFinal_ex(aesDecryptCtx, (unsigned char*) *decMsg + decLen, (int*) &blockLen))
	{
		return FAILURE;
	}
	decLen += blockLen;

	(*decMsg)[decLen] = '\0';

	EVP_CIPHER_CTX_cleanup(aesDecryptCtx);

	return (int)encMsgLen;
}

int Crypto::writeKeyToFile(FILE *fd, int key)
{
	switch (key)
	{
	case KEY_SERVER_PRI:
		if (!PEM_write_PrivateKey(fd, localKeypair, NULL, NULL, 0, 0, NULL))
		{
			return FAILURE;
		}
		break;

	case KEY_SERVER_PUB:
		if (!PEM_write_PUBKEY(fd, localKeypair))
		{
			return FAILURE;
		}
		break;

	case KEY_CLIENT_PUB:
		if (!PEM_write_PUBKEY(fd, remotePubKey))
		{
			return FAILURE;
		}
		break;

	case KEY_AES:
		fwrite(aesKey, 1, AES_KEYLEN, fd);
		break;

	case KEY_AES_IV:
		fwrite(aesIV, 1, AES_KEYLEN, fd);
		break;

	default:
		return FAILURE;
	}

	return SUCCESS;
}

int Crypto::getRemotePubKey(unsigned char **pubKey)
{
	BIO *bio = BIO_new(BIO_s_mem());
	PEM_write_bio_PUBKEY(bio, remotePubKey);

	int pubKeyLen = BIO_pending(bio);
	*pubKey = (unsigned char*) malloc(pubKeyLen);
	if (pubKey == NULL) return FAILURE;

	BIO_read(bio, *pubKey, pubKeyLen);

	// Insert the NUL terminator
	(*pubKey)[pubKeyLen - 1] = '\0';

	BIO_free_all(bio);

	return pubKeyLen;
}

int Crypto::setRemotePubKey(unsigned char* pubKey, size_t pubKeyLen)
{
	//BIO *bio = BIO_new(BIO_f_base64());
	BIO *bio = BIO_new(BIO_s_mem());
	if (BIO_write(bio, pubKey, (int)pubKeyLen) != (int) pubKeyLen)
	{
		return FAILURE;
	}

	RSA *_pubKey = RSA_new();
	if (_pubKey == NULL) return FAILURE;

	PEM_read_bio_PUBKEY(bio, &remotePubKey, NULL, NULL);

	BIO_free_all(bio);

	return SUCCESS;
}

int Crypto::getLocalPubKey(unsigned char** pubKey)
{
	BIO *bio = BIO_new(BIO_s_mem());
	PEM_write_bio_PUBKEY(bio, localKeypair);

	int pubKeyLen = BIO_pending(bio);
	*pubKey = (unsigned char*) malloc(pubKeyLen);
	if (pubKey == NULL) return FAILURE;

	BIO_read(bio, *pubKey, pubKeyLen);

	// Insert the NUL terminator
	(*pubKey)[pubKeyLen - 1] = '\0';

	BIO_free_all(bio);

	return pubKeyLen;
}

int Crypto::getLocalPriKey(unsigned char **priKey)
{
	BIO *bio = BIO_new(BIO_s_mem());

	PEM_write_bio_PrivateKey(bio, localKeypair, NULL, NULL, 0, 0, NULL);

	int priKeyLen = BIO_pending(bio);
	*priKey = (unsigned char*) malloc(priKeyLen + 1);
	if (priKey == NULL) return FAILURE;

	BIO_read(bio, *priKey, priKeyLen);

	// Insert the NUL terminator
	(*priKey)[priKeyLen] = '\0';

	BIO_free_all(bio);

	return priKeyLen;
}

int Crypto::getAESKey(unsigned char **aesKey)
{
	*aesKey = this->aesKey;
	return AES_KEYLEN / 8;
}

int Crypto::setAESKey(unsigned char *aesKey, size_t aesKeyLen)
{
	// Ensure the new key is the proper size
	if ((int) aesKeyLen != AES_KEYLEN / 8)
	{
		return FAILURE;
	}

	strncpy((char*) this->aesKey, (const char*) aesKey, AES_KEYLEN / 8);

	return SUCCESS;
}

int Crypto::setAESKey(string AesKey)
{
	Base64 codObj;
	string buff = codObj.base64_decode(AesKey);
	size_t aesKeyLen = buff.size();

	//aesKeyLen = base64Decode(aesKey.c_str(), &buffer);

	// Ensure the new key is the proper size
	if ((int) aesKeyLen != AES_KEYLEN / 8)
	{
		return FAILURE;
	}

	this->aesKey = new unsigned char[aesKeyLen];
	memcpy(this->aesKey, buff.c_str(), aesKeyLen);

	return SUCCESS;
}

int Crypto::getAESIv(unsigned char **aesIV)
{
	*aesIV = this->aesIV;
	return AES_KEYLEN / 16;
}

int Crypto::setAESIv(unsigned char *aesIV, size_t aesIVLen)
{
	// Ensure the new IV is the proper size
	if ((int) aesIVLen != AES_KEYLEN / 16)
	{
		return FAILURE;
	}

	strncpy((char*) this->aesIV, (const char*) aesIV, AES_KEYLEN / 16);

	return SUCCESS;
}

int Crypto::init()
{
	// Initalize contexts
	rsaEncryptCtx = EVP_CIPHER_CTX_new();
	aesEncryptCtx = EVP_CIPHER_CTX_new();

	rsaDecryptCtx = EVP_CIPHER_CTX_new();
	aesDecryptCtx = EVP_CIPHER_CTX_new();

	// Always a good idea to check if malloc failed
	if (rsaEncryptCtx == NULL || aesEncryptCtx == NULL || rsaDecryptCtx == NULL || aesDecryptCtx == NULL)
	{
		return FAILURE;
	}

	// Init these here to make valgrind happy
	EVP_CIPHER_CTX_init(rsaEncryptCtx);
	EVP_CIPHER_CTX_init(aesEncryptCtx);

	EVP_CIPHER_CTX_init(rsaDecryptCtx);
	EVP_CIPHER_CTX_init(aesDecryptCtx);

	// Init RSA
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);

	if (EVP_PKEY_keygen_init(ctx) <= 0)
	{
		return FAILURE;
	}

	if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, RSA_KEYLEN) <= 0)
	{
		return FAILURE;
	}

	if (EVP_PKEY_keygen(ctx, &localKeypair) <= 0)
	{
		return FAILURE;
	}

	EVP_PKEY_CTX_free(ctx);

	// Init AES
	aesKey = (unsigned char*) malloc(AES_KEYLEN / 8);
	aesIV = (unsigned char*) malloc(AES_KEYLEN / 8);

	unsigned char *aesPass = (unsigned char*) malloc(AES_KEYLEN / 8);
	unsigned char *aesSalt = (unsigned char*) malloc(8);

	if (aesKey == NULL || aesIV == NULL || aesPass == NULL || aesSalt == NULL)
	{
		return FAILURE;
	}

	// For the AES key we have the option of using a PBKDF (password-baswed key derivation formula)
	// or just using straight random data for the key and IV. Depending on your use case, you will
	// want to pick one or another.
#ifdef USE_PBKDF
	// Get some random data to use as the AES pass and salt
	if (RAND_bytes(aesPass, AES_KEYLEN / 8) == 0)
	{
		return FAILURE;
	}

	if (RAND_bytes(aesSalt, 8) == 0)
	{
		return FAILURE;
	}

	if (EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), aesSalt, aesPass, AES_KEYLEN / 8, AES_ROUNDS, aesKey, aesIV) == 0)
	{
		return FAILURE;
	}
#else
	if (RAND_bytes(aesKey, AES_KEYLEN / 8) == 0)
	{
		return FAILURE;
	}

	if (RAND_bytes(aesIV, AES_KEYLEN / 8) == 0)
	{
		return FAILURE;
	}
#endif

	return SUCCESS;
}

int Crypto::genTestClientKey()
{
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);

	if (EVP_PKEY_keygen_init(ctx) <= 0)
	{
		return FAILURE;
	}

	if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, RSA_KEYLEN) <= 0)
	{
		return FAILURE;
	}

	if (EVP_PKEY_keygen(ctx, &remotePubKey) <= 0)
	{
		return FAILURE;
	}

	EVP_PKEY_CTX_free(ctx);

	return SUCCESS;
}

int Crypto::AESEncrypt(string Message)
{
	// Encrypt the message with AES
	if ((this->encMsgLen = this->aesEncrypt((const unsigned char*) Message.c_str(), Message.size(), &this->encMsg)) == -1)
	{
		this->strError = "Error: ";
		return FAILURE;
	}

	// serializing unsigned char encrypted data and save to variable
	Base64 codObj;
	this->strEncryptMessage = codObj.base64_encode(this->encMsg, this->encMsgLen);

	return SUCCESS;
}

int Crypto::AESDecrypt(string Message)
{
	// un serializing data and Get length of unsigned char encrypted data
	Base64 codObj;
	Message = codObj.base64_decode(Message);

	// Decrypt the message with AES
	if ((this->decMsgLen = this->aesDecrypt((unsigned char *) Message.c_str(), Message.size(),  &this->decMsg)) == -1)
	{
		this->strError = "Error: ";
		return FAILURE;
	}

	// Save to variable
	this->strDecryptMessage = (char *) this->decMsg;

	return SUCCESS;
}

int Crypto::RSAEncrypt(string Message)
{
	// Encrypt the message with AES
	if ((this->encMsgLen = this->rsaEncrypt((const unsigned char*) Message.c_str(), Message.size(), &this->encMsg, NULL, 0, NULL, 0)) == -1)
	{
		this->strError = "Error: ";
		return FAILURE;
	}

	// serializing unsigned char encrypted data and save to variable
	Base64 codObj;
	this->strEncryptMessage = codObj.base64_encode(this->encMsg, this->encMsgLen);

	return SUCCESS;
}

int Crypto::RSADecrypt(string Message)
{
	// un serializing data and Get length of unsigned char encrypted data
	Base64 codObj;
	Message = codObj.base64_decode(Message);

	// Decrypt the message with AES
	if ((this->decMsgLen = this->rsaDecrypt((unsigned char*) Message.c_str(), Message.size(), NULL, 0, NULL, 0, &this->decMsg)) == -1)
	{
		this->strError = "Error: ";
		return FAILURE;
	}

	// Save to variable
	this->strDecryptMessage = (char *) this->decMsg;

	return SUCCESS;
}