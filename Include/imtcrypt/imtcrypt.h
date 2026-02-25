// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtcrypt.h
	\brief Main header file for the imtcrypt module - Cryptography and Encryption Services
	
	This file provides namespace-level documentation for the imtcrypt module.
	Include this file to access the complete imtcrypt module documentation in Doxygen.
*/

/**
	\namespace imtcrypt
	\brief Cryptography and Encryption Services Module
	
	The \b imtcrypt module provides comprehensive cryptographic services including
	symmetric and asymmetric encryption, hashing algorithms, and secure file
	persistence capabilities.
	
	\section imtcrypt_overview Overview
	
	This module serves as the security foundation for ImtCore applications,
	providing:
	- Symmetric encryption (AES-256)
	- Asymmetric encryption (RSA)
	- Hash generation (MD5, SHA)
	- Encrypted file persistence
	- Key management and provider infrastructure
	- SSL/TLS certificate handling
	
	\section imtcrypt_architecture Architecture
	
	\subsection imtcrypt_patterns Design Patterns
	
	<b>Strategy Pattern:</b>
	- Multiple encryption algorithms (AES, RSA) behind common IEncryption interface
	- Pluggable key providers through IEncryptionKeysProvider
	- Algorithm selection at runtime via EncryptionAlgorithm enum
	
	<b>Adapter Pattern:</b>
	- CEncryptionBasedPersistenceComp adapts IEncryption to IFilePersistence
	- Transparent encryption/decryption during file I/O operations
	- Seamless integration with existing persistence code
	
	<b>Provider Pattern:</b>
	- IEncryptionKeysProvider abstracts key storage and retrieval
	- CStaticEncryptionKeyProviderComp for compile-time keys
	- Supports runtime key injection and secure key storage
	
	\subsection imtcrypt_core_interfaces Core Interfaces
	
	The module is organized around several key interface hierarchies:
	
	<b>Encryption Hierarchy:</b>
	\code
	IEncryption (istd::IPolymorphic)
	├─ EncryptData() - Encrypt binary data with specified algorithm
	├─ DecryptData() - Decrypt binary data with specified algorithm
	└─ EncryptionAlgorithm enum: EA_RSA, EA_AES
	    │
	    └─ CEncryption (concrete implementation)
	        ├─ CAesEncryption (AES-256 symmetric encryption)
	        └─ CRsaEncryption (RSA asymmetric encryption)
	\endcode
	
	<b>Key Management Hierarchy:</b>
	\code
	IEncryptionKeysProvider (istd::IPolymorphic)
	├─ GetPublicKey() - Retrieve public key for encryption
	├─ GetPrivateKey() - Retrieve private key for decryption
	└─ GetSymmetricKey() - Retrieve symmetric key for AES
	    │
	    ├─ CStaticEncryptionKeyProviderComp (compile-time keys)
	    └─ Custom implementations for secure storage
	\endcode
	
	<b>Secure Persistence Hierarchy:</b>
	\code
	IEncryptedFilePersistence (ifile::IFilePersistence)
	├─ Extends standard file persistence with encryption
	├─ SaveToFile() - Encrypts before writing
	└─ LoadFromFile() - Decrypts after reading
	    │
	    └─ CEncryptionBasedPersistenceComp
	        ├─ Wraps IEncryption service
	        ├─ Uses IEncryptionKeysProvider for keys
	        └─ Transparent encryption/decryption
	\endcode
	
	<b>Hashing Services:</b>
	\code
	IHashGenerator (istd::IPolymorphic)
	├─ GenerateHash() - Generate hash from binary data
	└─ Multiple algorithms (MD5, SHA-1, SHA-256)
	    │
	    └─ CMD5HashCalculator (MD5 implementation)
	\endcode
	
	\section imtcrypt_algorithms Supported Algorithms
	
	\subsection imtcrypt_symmetric Symmetric Encryption (AES)
	
	**CAesEncryption / CAesKey:**
	- Algorithm: AES-256 in CBC mode
	- Key size: 256 bits (32 bytes)
	- Initialization Vector (IV): 128 bits (16 bytes)
	- Padding: PKCS#7
	- Thread-safe: Yes
	
	**Usage:**
	\code{.cpp}
	// Create AES key (32 bytes for AES-256)
	QByteArray keyData(32, 0);
	// ... fill keyData with secure random bytes or derive from password
	
	CAesKey aesKey(keyData);
	CAesEncryption aes;
	
	// Encrypt data
	QByteArray plaintext = "Sensitive data";
	QByteArray ciphertext = aes.Encrypt(plaintext, aesKey);
	
	// Decrypt data
	QByteArray decrypted = aes.Decrypt(ciphertext, aesKey);
	Q_ASSERT(decrypted == plaintext);
	\endcode
	
	\subsection imtcrypt_asymmetric Asymmetric Encryption (RSA)
	
	**CRsaEncryption / CRsaKey:**
	- Algorithm: RSA with PKCS#1 v1.5 padding
	- Key sizes: 1024, 2048, 4096 bits
	- Format: PEM encoding
	- Use cases: Key exchange, digital signatures
	
	**Usage:**
	\code{.cpp}
	// Load RSA keys (PEM format)
	QFile publicKeyFile("public.pem");
	publicKeyFile.open(QIODevice::ReadOnly);
	CRsaKey publicKey(publicKeyFile.readAll());
	
	QFile privateKeyFile("private.pem");
	privateKeyFile.open(QIODevice::ReadOnly);
	CRsaKey privateKey(privateKeyFile.readAll());
	
	// Encrypt with public key
	CRsaEncryption rsa;
	QByteArray plaintext = "Secret message";
	QByteArray encrypted = rsa.EncryptWithPublicKey(plaintext, publicKey);
	
	// Decrypt with private key
	QByteArray decrypted = rsa.DecryptWithPrivateKey(encrypted, privateKey);
	Q_ASSERT(decrypted == plaintext);
	\endcode
	
	\subsection imtcrypt_hashing Hash Functions
	
	**CMD5HashCalculator:**
	- Algorithm: MD5 (128-bit hash)
	- Use cases: Checksums, non-cryptographic integrity checks
	- Warning: NOT recommended for password storage or digital signatures
	
	**Usage:**
	\code{.cpp}
	CMD5HashCalculator md5;
	QByteArray data = "Data to hash";
	QByteArray hash = md5.GenerateHash(data);
	
	// Hash is 16 bytes (128 bits)
	Q_ASSERT(hash.size() == 16);
	
	// Convert to hex string for display
	QString hashHex = hash.toHex();
	\endcode
	
	\section imtcrypt_persistence Encrypted File Persistence
	
	\subsection imtcrypt_persistence_basic Basic Usage
	
	**CEncryptionBasedPersistenceComp** provides transparent encryption for file
	operations, adapting any IFilePersistence-compatible object:
	
	\code{.cpp}
	// Setup encryption service
	auto encryption = CEncryption::CreateInstance();
	
	// Setup key provider (static keys for this example)
	auto keyProvider = CStaticEncryptionKeyProviderComp::CreateInstance();
	// ... configure keys in keyProvider
	
	// Create encrypted persistence wrapper
	auto encryptedPersistence = CEncryptionBasedPersistenceComp::CreateInstance();
	encryptedPersistence->SetEncryption(encryption.get());
	encryptedPersistence->SetKeysProvider(keyProvider.get());
	encryptedPersistence->SetAlgorithm(IEncryption::EA_AES);
	
	// Use like normal file persistence
	MyDataObject dataObj;
	encryptedPersistence->SaveToFile(&dataObj, "encrypted_data.bin");
	
	// Load automatically decrypts
	MyDataObject loaded;
	encryptedPersistence->LoadFromFile(&loaded, "encrypted_data.bin");
	\endcode
	
	\subsection imtcrypt_persistence_integration Integration Patterns
	
	**Pattern 1: Wrapper around existing persistence:**
	\code{.cpp}
	// Have existing persistence
	auto jsonPersistence = CJsonPersistenceComp::CreateInstance();
	
	// Wrap it with encryption
	auto encrypted = CEncryptionBasedPersistenceComp::CreateInstance();
	encrypted->SetInnerPersistence(jsonPersistence.get());
	encrypted->SetEncryption(myEncryption);
	encrypted->SetKeysProvider(myKeyProvider);
	
	// Now SaveToFile encrypts JSON data automatically
	\endcode
	
	**Pattern 2: Component-based dependency injection:**
	\code{.cpp}
	// In component constructor or initialization
	class CSecureDataManagerComp : public ACF_COMPONENT(IDataManager)
	{
	    I_REFERENCE(IEncryption, m_encryption)
	    I_REFERENCE(IEncryptionKeysProvider, m_keyProvider)
	    
	    void InitializeComponent() override
	    {
	        m_persistence->SetEncryption(m_encryption);
	        m_persistence->SetKeysProvider(m_keyProvider);
	    }
	};
	\endcode
	
	\section imtcrypt_key_management Key Management
	
	\subsection imtcrypt_static_keys Static Keys
	
	**CStaticEncryptionKeyProviderComp** - For development or embedded scenarios:
	
	\code{.cpp}
	auto keyProvider = CStaticEncryptionKeyProviderComp::CreateInstance();
	
	// Set AES key (32 bytes for AES-256)
	QByteArray aesKey(32, 0);
	// ... generate or load key
	keyProvider->SetSymmetricKey(aesKey);
	
	// Set RSA keys (PEM format)
	QFile pubFile("public.pem");
	pubFile.open(QIODevice::ReadOnly);
	keyProvider->SetPublicKey(pubFile.readAll());
	
	QFile privFile("private.pem");
	privFile.open(QIODevice::ReadOnly);
	keyProvider->SetPrivateKey(privFile.readAll());
	\endcode
	
	\subsection imtcrypt_secure_keys Secure Key Storage
	
	For production systems, implement custom IEncryptionKeysProvider:
	
	\code{.cpp}
	class CSecureKeyProviderComp : public ACF_COMPONENT(IEncryptionKeysProvider)
	{
	public:
	    QByteArray GetSymmetricKey() const override
	    {
	        // Load from secure storage:
	        // - Windows: Data Protection API (DPAPI)
	        // - Linux: Keyring (libsecret)
	        // - macOS: Keychain
	        // - Hardware: TPM, HSM
	        return LoadFromSecureStorage();
	    }
	    
	    QByteArray GetPublicKey() const override
	    {
	        // Load from certificate store or file
	        return LoadCertificate();
	    }
	};
	\endcode
	
	\section imtcrypt_security_considerations Security Considerations
	
	\subsection imtcrypt_best_practices Best Practices
	
	**Key Generation:**
	- Use QRandomGenerator::system()->generate() for cryptographic keys
	- Never hardcode keys in source code (use secure storage)
	- Minimum key sizes: AES-256 (32 bytes), RSA-2048 (2048 bits)
	
	**Key Storage:**
	- Use platform-specific secure storage (Keychain, DPAPI, Keyring)
	- Never store keys in plain text files
	- Consider hardware security modules (HSM) for critical applications
	
	**Algorithm Selection:**
	- AES-256 for symmetric encryption (file contents, data at rest)
	- RSA-2048+ for asymmetric encryption (key exchange, signatures)
	- SHA-256+ for hashing (never MD5 for security-critical operations)
	
	**Implementation Guidelines:**
	- Always use authenticated encryption (consider AES-GCM instead of CBC)
	- Generate new IV for each encryption operation
	- Use constant-time comparison for authentication tags
	- Clear sensitive data from memory after use
	
	\subsection imtcrypt_limitations Known Limitations
	
	- MD5 is provided for legacy compatibility only (NOT cryptographically secure)
	- CBC mode without authentication is vulnerable to padding oracle attacks
	- RSA PKCS#1 v1.5 padding has known weaknesses (consider OAEP)
	- No built-in password-based key derivation (use PBKDF2/Argon2 externally)
	
	\section imtcrypt_integration Integration with Other Modules
	
	<b>With imtlic (Licensing):</b>
	- Encrypt license keys and activation data
	- Protect license files from tampering
	- Secure hardware binding information
	
	<b>With imtauth (Authentication):</b>
	- Hash passwords before storage
	- Encrypt authentication tokens
	- Secure session data persistence
	
	<b>With imtdb (Database):</b>
	- Encrypt sensitive database fields
	- Secure database connection credentials
	- Protect backup files with encryption
	
	<b>With imtfile (File I/O):</b>
	- Transparent encryption through CEncryptionBasedPersistenceComp
	- Automatic encryption for IFilePersistence objects
	- Format-independent encrypted file storage
	
	\section imtcrypt_examples Complete Examples
	
	\subsection imtcrypt_example_license Encrypted License File
	
	\code{.cpp}
	// Save license with encryption
	void SaveEncryptedLicense(const ILicenseInstance* license)
	{
	    // Setup encryption
	    auto encryption = CEncryption::CreateInstance();
	    auto keyProvider = LoadProductKeyProvider(); // From secure storage
	    
	    auto encryptedPersistence = CEncryptionBasedPersistenceComp::CreateInstance();
	    encryptedPersistence->SetEncryption(encryption.get());
	    encryptedPersistence->SetKeysProvider(keyProvider.get());
	    encryptedPersistence->SetAlgorithm(IEncryption::EA_AES);
	    
	    // Save (automatically encrypted)
	    QString licenseFile = QStandardPaths::writableLocation(
	        QStandardPaths::AppDataLocation) + "/license.dat";
	    encryptedPersistence->SaveToFile(license, licenseFile);
	}
	
	// Load license with decryption
	ILicenseInstanceUniquePtr LoadEncryptedLicense()
	{
	    auto encryption = CEncryption::CreateInstance();
	    auto keyProvider = LoadProductKeyProvider();
	    
	    auto encryptedPersistence = CEncryptionBasedPersistenceComp::CreateInstance();
	    encryptedPersistence->SetEncryption(encryption.get());
	    encryptedPersistence->SetKeysProvider(keyProvider.get());
	    encryptedPersistence->SetAlgorithm(IEncryption::EA_AES);
	    
	    auto license = CLicenseInstance::CreateInstance();
	    encryptedPersistence->LoadFromFile(license.get(), licenseFile);
	    
	    return license;
	}
	\endcode
	
	\subsection imtcrypt_example_config Encrypted Configuration
	
	\code{.cpp}
	// Application settings with sensitive data
	class CSecureSettingsComp : public ACF_COMPONENT(IAppSettings)
	{
	    I_REFERENCE(IEncryption, m_encryption)
	    I_REFERENCE(IEncryptionKeysProvider, m_keyProvider)
	    
	    void SaveSettings() override
	    {
	        // Serialize settings to JSON
	        QJsonObject settings;
	        settings["apiKey"] = m_apiKey;
	        settings["databasePassword"] = m_dbPassword;
	        
	        QJsonDocument doc(settings);
	        QByteArray jsonData = doc.toJson();
	        
	        // Encrypt
	        QByteArray encrypted;
	        m_encryption->EncryptData(
	            jsonData, 
	            IEncryption::EA_AES, 
	            *m_keyProvider, 
	            encrypted);
	        
	        // Save to file
	        QFile file(GetSettingsPath());
	        file.open(QIODevice::WriteOnly);
	        file.write(encrypted);
	    }
	    
	    void LoadSettings() override
	    {
	        // Load encrypted data
	        QFile file(GetSettingsPath());
	        file.open(QIODevice::ReadOnly);
	        QByteArray encrypted = file.readAll();
	        
	        // Decrypt
	        QByteArray decrypted;
	        m_encryption->DecryptData(
	            encrypted, 
	            IEncryption::EA_AES, 
	            *m_keyProvider, 
	            decrypted);
	        
	        // Parse JSON
	        QJsonDocument doc = QJsonDocument::fromJson(decrypted);
	        QJsonObject settings = doc.object();
	        m_apiKey = settings["apiKey"].toString();
	        m_dbPassword = settings["databasePassword"].toString();
	    }
	};
	\endcode
	
	\section imtcrypt_testing Testing and Debugging
	
	\subsection imtcrypt_test_vectors Test Vectors
	
	Use known test vectors to verify encryption implementation:
	
	\code{.cpp}
	void TestAesEncryption()
	{
	    // NIST test vector for AES-256
	    QByteArray key = QByteArray::fromHex(
	        "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4");
	    QByteArray plaintext = QByteArray::fromHex(
	        "6bc1bee22e409f96e93d7e117393172a");
	    QByteArray expectedCiphertext = QByteArray::fromHex(
	        "f58c4c04d6e5f1ba779eabfb5f7bfbd6");
	    
	    CAesKey aesKey(key);
	    CAesEncryption aes;
	    
	    QByteArray ciphertext = aes.Encrypt(plaintext, aesKey);
	    Q_ASSERT(ciphertext == expectedCiphertext);
	}
	\endcode
	
	\subsection imtcrypt_debugging Debugging Tips
	
	- Log encryption operations in debug builds (but never log keys!)
	- Use hex encoding (QByteArray::toHex()) to display binary data
	- Verify key sizes before encryption operations
	- Check IV generation (must be unique per encryption)
	- Test round-trip: encrypt → decrypt → verify original data
	
	\section imtcrypt_references References
	
	<b>Related Modules:</b>
	- imtlic - License management with encryption support
	- imtauth - Authentication with password hashing
	- imtfile - File I/O operations
	- imtdb - Database with encrypted field support
	
	<b>Standards and Specifications:</b>
	- NIST FIPS 197 - AES Specification
	- RFC 3447 - RSA Cryptography Specifications (PKCS #1)
	- RFC 1321 - MD5 Message-Digest Algorithm
	- NIST SP 800-38A - Block Cipher Modes of Operation
	
	<b>External Documentation:</b>
	- Qt Cryptographic Architecture: https://doc.qt.io/qt-6/topics-security.html
	- OpenSSL Documentation: https://www.openssl.org/docs/
*/

namespace imtcrypt
{
	// Namespace for cryptography and encryption services
}
