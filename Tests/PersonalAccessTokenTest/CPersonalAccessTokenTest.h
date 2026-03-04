#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <ipackage/CComponentAccessor.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtauth/IPersonalAccessTokenManager.h>


/**
	Test class for personal access token functionality.
	Tests token creation, validation, revocation, and expiration.
*/
class CPersonalAccessTokenTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();
	void testTokenCreation();
	void testTokenValidation();
	void testTokenRevocation();
	void testTokenExpiration();
	void testGetTokenIds();
	void testUpdateLastUsedAt();
	void testDeleteToken();

private:
	QString m_imtCorePath;
	QString m_registryFile;
	QString m_configFile;
	istd::TDelPtr<ipackage::CComponentAccessor> m_compositePtr;
	imtauth::IPersonalAccessTokenManager* m_tokenManagerPtr;
};


