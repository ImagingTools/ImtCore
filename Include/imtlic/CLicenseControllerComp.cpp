#include <imtlic/CLicenseControllerComp.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QStandardPaths>

// ACF includes
#include <istd/CCrcCalculator.h>
#include <istd/CSystem.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/CProductInstanceInfo.h>


namespace imtlic
{


// public methods

CLicenseControllerComp::CLicenseControllerComp()
	:m_licenseKeysProvider(*this),
	m_isInitializing(false)
{
}


// protected methods

void CLicenseControllerComp::OnFingeprintCheckTimer()
{
	SendVerboseMessage(tr("Checking license fingerprint"), tr("License Controller"));

	imtlic::CProductInstanceInfo tempLicense;
	bool retVal = ReadLicenseFromFile(tempLicense);
	if (!retVal){
		if (m_productInstanceCompPtr.IsValid()){
			m_productInstanceCompPtr->ResetData();
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CLicenseControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_isInitializing = true;

	m_licenseKeysProvider.RegisterObject(m_licenseKeysProviderCompPtr.GetPtr(), &CLicenseControllerComp::OnLicenseKeysUpdated);

	m_isInitializing = false;
}


void CLicenseControllerComp::OnComponentDestroyed()
{
	m_licenseKeysProvider.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CLicenseControllerComp::ReadLicenseFromFile(imtlic::IProductInstanceInfo& license) const
{
	if (!m_licensePathCompPtr.IsValid()){
		SendCriticalMessage(0, "No license path component was set. Please check component configuration", "CLicenseControllerComp::ReadLicenseFile");

		return false;
	}

	if (!m_productInstancePersistenceCompPtr.IsValid()){
		SendCriticalMessage(0, "No product instance persistence component was set. Please check component configuration", "CLicenseControllerComp::ReadLicenseFile");

		return false;
	}

	QString licenseFilePath = m_licensePathCompPtr->GetPath();
	if (licenseFilePath.isEmpty()){
		SendCriticalMessage(0, "License file path is empty. Please check component configuration", "CLicenseControllerComp::ReadLicenseFile");

		return false;
	}

	QFileInfo licenseFileInfo(licenseFilePath);
	if (!licenseFileInfo.exists()){
		SendErrorMessage(0, QString(QObject::tr("License file '%1' doesn't exist")).arg(licenseFilePath), "License Management");

		return false;
	}

	QString fingerprintFilePath = GetFingerprintPath();

	int state = m_productInstancePersistenceCompPtr->LoadFromFile(license, licenseFilePath);
	if (state != ifile::IFilePersistence::OS_OK){
		int daysUntilExpire = -1;
		if (LoadFingerprint(fingerprintFilePath, license, daysUntilExpire)){
			SendWarningMessage(0, QString(QObject::tr("You have no valid license to run this software anymore. You have %1 day(s) to update your system with a valid license")).arg(daysUntilExpire), "License Management");
		}
		else{
			SendErrorMessage(0, QString(QObject::tr("You have no license to run this software. License file: '%1'")).arg(licenseFilePath), "License Management");

			return false;
		}
	}
	else{
		if (!UpdateFingerprint(fingerprintFilePath, license)){
			SendErrorMessage(0, QString(QObject::tr("License fingerprint could not be updated")), "License Management");

			return false;
		}

		SendInfoMessage(0, QString(QObject::tr("License was successfully loaded")), "License Management");
	}

	int oneHour = 1000 * 60 * 60;

	QTimer::singleShot(oneHour, this, &CLicenseControllerComp::OnFingeprintCheckTimer);

	return true;
}


bool CLicenseControllerComp::UpdateFingerprint(const QString& fingerprintFilePath, imtlic::IProductInstanceInfo& license) const
{
	if (!m_fingerprintInstancePersistenceCompPtr.IsValid()){
		return false;
	}

	return (m_fingerprintInstancePersistenceCompPtr->SaveToFile(license, fingerprintFilePath) == ifile::IFilePersistence::OS_OK);
}


bool CLicenseControllerComp::LoadFingerprint(const QString& filePath, imtlic::IProductInstanceInfo& license, int& daysUntilExpire) const
{
	daysUntilExpire = -1;

	if (!m_fingerprintInstancePersistenceCompPtr.IsValid()){
		return false;
	}

	QFileInfo fingerprintInfo(filePath);
	if (!fingerprintInfo.exists()){
		return false;
	}

	if (m_fingerprintExpirationAttrPtr.IsValid()){
		QDateTime currentTime = QDateTime::currentDateTime();
		QDateTime fingerprintTimeStamp = fingerprintInfo.lastModified();

		int days = fingerprintTimeStamp.daysTo(currentTime);
		if (days < *m_fingerprintExpirationAttrPtr){
			daysUntilExpire = *m_fingerprintExpirationAttrPtr - days;

			return (m_fingerprintInstancePersistenceCompPtr->LoadFromFile(license, filePath) == ifile::IFilePersistence::OS_OK);
		}
	}

	return false;
}


QString CLicenseControllerComp::GetFingerprintPath() const
{
	QString licenseFilePath = m_licensePathCompPtr->GetPath();
	if (licenseFilePath.isEmpty()){
		return QString();
	}

	QFileInfo licenseFileInfo(licenseFilePath);
	if (!licenseFileInfo.exists()){
		return QString();
	}

	int checksum = istd::CCrcCalculator::GetCrcFromFile(licenseFilePath);

	QString checkSumString = QString::number(checksum, 16).rightJustified(8, '0').toUpper();

	QString fingerprintFolder = m_licenseFingerprintPathCompPtr.IsValid() ?
				m_licenseFingerprintPathCompPtr->GetPath() :
				QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);

	if (istd::CSystem::EnsurePathExists(fingerprintFolder)){
		QString fingerprintFilePath = fingerprintFolder + "/"  + checkSumString + ".xli";

		return fingerprintFilePath;
	}

	return QString();
}


void CLicenseControllerComp::OnLicenseKeysUpdated(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtcrypt::IEncryptionKeysProvider* licenseKeysProviderPtr)
{
	if (!m_isInitializing && m_productInstanceCompPtr.IsValid()){
		Q_ASSERT(licenseKeysProviderPtr != nullptr);

		QByteArray key = licenseKeysProviderPtr->GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KT_PASSWORD);
		if (!key.isEmpty()){
			SendInfoMessage(0, tr("License key was changed. Checking license fingerprint"), tr("License Controller"));

			ReadLicenseFromFile(*m_productInstanceCompPtr);
		}
		else{
			// Remove all existing license data:
			m_productInstanceCompPtr->ResetData();
		}
	}
}


} // namespace imtlic


