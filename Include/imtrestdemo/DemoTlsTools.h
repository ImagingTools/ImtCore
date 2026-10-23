// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtNetwork/QSsl>
#include <QtNetwork/QSslCertificate>


namespace imtrestdemo
{


/**
	Tools of the TLS of the demo: the check of the files that the server is asked to use and the test certificates that are made by the
	program \c openssl.
*/
namespace DemoTlsTools
{


/**
	Names of the sets of the test certificates (see GenerateSamples()).
*/
extern const char* const s_singleSampleId;
extern const char* const s_chainSampleId;
extern const char* const s_severalSampleId;
extern const char* const s_bundleSampleId;
extern const char* const s_ecSampleId;
extern const char* const s_wrongHostSampleId;

/**
	The TLS of the demo must work through OpenSSL: the other TLS libraries of Qt (Secure Transport of macOS) put the certificate and the key of a
	server to the keychain of the user, and the demo must not change the system. On macOS the library of OpenSSL is not on the paths of the
	system, so the program is started again with the path of the library that is near the program \c openssl (the environment variable
	\c IMTREST_OPENSSL_LIB_DIR gives the directory directly). Call it first in \c main(), before the application of Qt is created, with the
	original arguments. Nothing is done on the other systems, if the library is not found, or if the program is already started again.
*/
void PrepareOpensslBackend(char* argv[]);

/**
	Make OpenSSL the TLS backend of Qt. \return \c false if it is not available, the reason is in \c errorText: the TLS must not be
	used with the other backends.
*/
bool UseOpensslBackend(QString& errorText);

/**
	Find the program \c openssl: the path that is given, then the variable of the environment \c IMTREST_OPENSSL, then \c PATH.
	\return Empty string if the program is not found.
*/
QString FindOpenssl(const QString& explicitPath = QString());

/**
	Read the certificates of a PEM file: all of them, in the order of the file.
	\return Empty list if the file is not readable or has no certificates, the reason is in \c errorText.
*/
QList<QSslCertificate> ReadCertificates(const QString& path, QString& errorText);

/**
	Read the private key of a PEM file: RSA, EC and DSA are tried.
	\return \c false if the file is not readable or is not a private key.
*/
bool ReadPrivateKeyAlgorithm(const QString& path, QSsl::KeyAlgorithm& algorithm, QString& errorText);

/**
	Check the files that a server is asked to use: the certificate file (it can have the whole chain), the files of the intermediate
	certificates and the file of the key. Everything must exist, be read and be what it is called.
*/
bool ValidateServerFiles(const QString& certificateFile, const QStringList& chainFiles, const QString& keyFile, QString& errorText);

/**
	Make the sets of the test certificates in the directory with the program \c openssl. The authorities (root and two intermediate ones)
	are in \c ca, every set of the server is in its own directory:
	- \c single: one self-signed certificate;
	- \c chain: the certificate of the server, signed by an intermediate authority; the file of the intermediate certificate is separate;
	- \c several: the certificate of the server and two intermediate certificates, every one in its own file;
	- \c bundle: one file with the certificate of the server and both intermediate certificates (\c fullchain.pem);
	- \c ec: a self-signed certificate with an EC key;
	- \c wrong-host: a self-signed certificate for another host name.
	Every certificate has \c localhost and \c 127.0.0.1 in the alternative names (except \c wrong-host). The certificate files and
	the files of the keys are PEM.
	\return \c false if openssl failed, the reason is in \c errorText.
*/
bool GenerateSamples(const QString& directory, const QString& opensslPath, QString& errorText);

/**
	Keep the certificates and the key that a client has sent (the content of PEM files) in a new directory of the storage, so the server can use them:
	every call makes its own directory, the directory has the rights of the owner only, the key is readable by the owner only. Everything is checked
	before the files are written: the certificates must be PEM certificates, the key must be a PEM private key and must not be in the place of a
	certificate. The storage must not be inside of the directory of the files that the server gives to everybody.
	\return \c false if something is wrong, nothing is written in this case.
*/
bool StorePem(
			const QString& storageDirectory,
			const QString& certificatePem,
			const QStringList& chainPem,
			const QString& keyPem,
			QString& certificateFile,
			QStringList& chainFiles,
			QString& keyFile,
			QString& errorText);

/**
	Remove the directories of the storage that do not have any of the files (see StorePem()).
*/
void RemoveUnusedStoredFiles(const QString& storageDirectory, const QStringList& keptFiles);

/**
	Describe the sets that exist in the directory: a list of the objects with \c id, \c title, \c description, \c certificateFile,
	\c chainFiles, \c keyFile, \c trustedFiles (what a client must trust), \c trustedPem (the same, the content of the files: a client
	that is not on the machine of the server needs it) and \c certificatesInChain (how many certificates the
	server sends).
*/
QJsonArray DescribeSamples(const QString& directory);


} // namespace DemoTlsTools


} // namespace imtrestdemo

