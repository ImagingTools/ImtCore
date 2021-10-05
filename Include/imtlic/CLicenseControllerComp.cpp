#include <imtlic/CLicenseControllerComp.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

// ACF includes
#include <istd/CCrcCalculator.h>
#include <istd/CSystem.h>
#include <istd/CChangeGroup.h>
#include <istd/CChangeNotifier.h>
#include <ilog/CMessage.h>

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


// reimplemented (ILicenseController)

bool CLicenseControllerComp::ImportLicense(const QString& licenseFilePath, ilog::IMessageConsumer* logPtr) const
{
	if (m_licensePathCompPtr.IsValid() && m_productInstancePersistenceCompPtr.IsValid() && m_productInstanceCompPtr.IsValid()){
		QString targetFilePath = m_licensePathCompPtr->GetPath();

		QFileInfo licenseFileInfo(targetFilePath);

		if (!targetFilePath.isEmpty()){
			// Backup the license file:
			QString backupFilePath;
			if (licenseFileInfo.exists()){
				backupFilePath = QDir::tempPath() + "/" + QUuid::createUuid().toString(QUuid::WithoutBraces) + "/License.lic";
				if (!istd::CSystem::FileCopy(targetFilePath, backupFilePath)){
					if (logPtr != nullptr){
						ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, tr("Backup of the existing license failed. Import canceled"), "License Manager"));

						logPtr->AddMessage(messagePtr);
					}

					return false;
				}
			}

			if (istd::CSystem::FileCopy(licenseFilePath, targetFilePath, true)){
				int licenseState = m_productInstancePersistenceCompPtr->LoadFromFile(*m_productInstanceCompPtr, targetFilePath);
				if (licenseState == ifile::IFilePersistence::OS_OK){
					OnFingeprintCheckTimer();

					return true;
				}
				else{
					if (logPtr != nullptr){
						ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, tr("License file could not be imported"), "License Manager"));

						logPtr->AddMessage(messagePtr);
					}

					// Restore the license backup:
					if (!backupFilePath.isEmpty()){
						if (istd::CSystem::FileCopy(backupFilePath, targetFilePath, true)){
							QFile::remove(backupFilePath);
						}
						else{
							if (logPtr != nullptr){
								ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, tr("Restore of the last license failed"), "License Manager"));

								logPtr->AddMessage(messagePtr);
							}
						}
					}
				}
			}
			else{
				if (logPtr != nullptr){
					ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, tr("License file could not be copied to the target location"), "License Manager"));

					logPtr->AddMessage(messagePtr);
				}
			}
		}
	}

	return false;
}


// protected methods

void CLicenseControllerComp::OnFingeprintCheckTimer() const
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

	if (m_licensePathCompPtr.IsValid()){
		m_licenseStatus.SetLicenseLocation(m_licensePathCompPtr->GetPath());
	}

	m_isInitializing = true;

	m_licenseKeysProvider.RegisterObject(m_licenseKeysProviderCompPtr.GetPtr(), &CLicenseControllerComp::OnLicenseKeysUpdated);

	m_isInitializing = false;

	int oneHour = 1000 * 60 * 60;
	QTimer::singleShot(oneHour, this, &CLicenseControllerComp::OnFingeprintCheckTimer);

	connect(&m_checkLicenseTimer, &QTimer::timeout, this, &CLicenseControllerComp::OnFingeprintCheckTimer);

	m_checkLicenseTimer.start(oneHour);
}


void CLicenseControllerComp::OnComponentDestroyed()
{
	m_licenseKeysProvider.UnregisterAllObjects();

	m_checkLicenseTimer.stop();

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CLicenseControllerComp::ReadLicenseFromFile(imtlic::IProductInstanceInfo& license) const
{
	istd::CChangeGroup changeGroup(&m_licenseStatus);

	m_licenseStatus.SetLicenseStatusFlags(imtlic::ILicenseStatus::LSF_LICENSE_INVALID);

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

		m_licenseStatus.SetLicenseStatusFlags(imtlic::ILicenseStatus::LSF_LICENSE_INVALID | imtlic::ILicenseStatus::LSF_NO_LICENSE);

		return false;
	}

	QFileInfo licenseFileInfo(licenseFilePath);
	if (!licenseFileInfo.exists()){
		SendErrorMessage(0, QString(QObject::tr("License file '%1' doesn't exist")).arg(licenseFilePath), "License Management");

		m_licenseStatus.SetLicenseStatusFlags(imtlic::ILicenseStatus::LSF_LICENSE_INVALID | imtlic::ILicenseStatus::LSF_NO_LICENSE);

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
			SendWarningMessage(0, QString(QObject::tr("License fingerprint could not be updated")), "License Management");
		}

		SendInfoMessage(0, QString(QObject::tr("License was successfully loaded")), "License Management");

		m_licenseStatus.SetLicenseStatusFlags(imtlic::ILicenseStatus::LSF_LICENSE_VALID);
	}

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


// public methods of the embedded class LicenseStatus

CLicenseControllerComp::LicenseStatus::LicenseStatus()
	:m_licenseStatusFlags(LSF_LICENSE_INVALID)
{
}


// reimplemented (imtlic::ILicenseStatus)

int CLicenseControllerComp::LicenseStatus::GetLicenseStatusFlags() const
{
	return m_licenseStatusFlags;
}


void CLicenseControllerComp::LicenseStatus::SetLicenseStatusFlags(int licenseStatusFlags)
{
	if (m_licenseStatusFlags != licenseStatusFlags){
		istd::CChangeNotifier changeNotifier(this);

		m_licenseStatusFlags = licenseStatusFlags;
	}
}


QString CLicenseControllerComp::LicenseStatus::GetLicenseLocation() const
{
	return m_licenseLocation;
}


void CLicenseControllerComp::LicenseStatus::SetLicenseLocation(const QString& licenseLocation)
{
	if (m_licenseLocation != licenseLocation){
		istd::CChangeNotifier changeNotifier(this);

		m_licenseLocation = licenseLocation;
	}
}


} // namespace imtlic


