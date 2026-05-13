// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
#include <imtbase/imtbase.h>
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
						ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, QT_TR_NOOP("Backup of the existing license failed. Import canceled"), "License Manager"));

						logPtr->AddMessage(messagePtr);
					}

					return false;
				}
			}

			if (istd::CSystem::FileCopy(licenseFilePath, targetFilePath, true)){
				// if multiple keys are defined, select the correct one, before trying to load the license
				if (m_keyRingCompPtr.IsValid()){
					m_keyRingCompPtr->SelectKeyFor(licenseFilePath);
				}

				int licenseState = m_productInstancePersistenceCompPtr->LoadFromFile(*m_productInstanceCompPtr, targetFilePath);
				if (licenseState == ifile::IFilePersistence::OS_OK){
					OnFingeprintCheckTimer();

					return true;
				}
				else{
					if (logPtr != nullptr){
						QString addtionalMessage;
						if (m_additionalImportLicenseErrorMessageAttrPtr.IsValid()){
							addtionalMessage = *m_additionalImportLicenseErrorMessageAttrPtr;
						}

						QString errorMessage = QT_TR_NOOP("License file could not be imported");
						if (!addtionalMessage.isEmpty()){
							errorMessage += ".\n" + addtionalMessage;
						}

						ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, errorMessage, "License Manager"));

						logPtr->AddMessage(messagePtr);
					}

					// Restore the license backup:
					if (!backupFilePath.isEmpty()){
						if (istd::CSystem::FileCopy(backupFilePath, targetFilePath, true)){
							QFile::remove(backupFilePath);
						}
						else{
							if (logPtr != nullptr){
								ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, QT_TR_NOOP("Restore of the last license failed"), "License Manager"));

								logPtr->AddMessage(messagePtr);
							}
						}
					}
				}
			}
			else{
				if (logPtr != nullptr){
					ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, QT_TR_NOOP("License file could not be copied to the target location"), "License Manager"));

					logPtr->AddMessage(messagePtr);
				}
			}
		}
	}

	return false;
}


bool CLicenseControllerComp::CheckLicense(const QByteArray& key) const
{
	if (!m_licensePathCompPtr.IsValid()){
		SendCriticalMessage(0,QT_TR_NOOP("No license path component was set. Please check component configuration"), "CLicenseControllerComp::CheckLicense");

		return false;
	}

	if (!m_productInstancePersistenceCompPtr.IsValid()){
		SendCriticalMessage(0, QT_TR_NOOP("No product instance persistence component was set. Please check component configuration"), "CLicenseControllerComp::CheckLicense");

		return false;
	}

	if (!m_encryptedInstancePersistenceCompPtr.IsValid()){
		SendCriticalMessage(0, QT_TR_NOOP("No encrypted instance persistence component was set. Please check component configuration"), "CLicenseControllerComp::CheckLicense");

		return false;
	}

	QString licenseFilePath = m_licensePathCompPtr->GetPath();
	if (licenseFilePath.isEmpty()){
		SendCriticalMessage(0, QT_TR_NOOP("License file path is empty. Please check component configuration"), "CLicenseControllerComp::CheckLicense");

		m_licenseStatus.SetLicenseStatusFlags(imtlic::ILicenseStatus::LSF_LICENSE_INVALID | imtlic::ILicenseStatus::LSF_NO_LICENSE);

		return false;
	}

	QFileInfo licenseFileInfo(licenseFilePath);
	if (!licenseFileInfo.exists()){
		SendErrorMessage(0, QT_TR_NOOP(QString("License file '%1' doesn't exist").arg(licenseFilePath)), "License Management");

		m_licenseStatus.SetLicenseStatusFlags(imtlic::ILicenseStatus::LSF_LICENSE_INVALID | imtlic::ILicenseStatus::LSF_NO_LICENSE);

		return false;
	}

	imtlic::CProductInstanceInfo tempLicense;
	int state = m_encryptedInstancePersistenceCompPtr->LoadFromEncryptedFile(key, licenseFilePath, tempLicense);

	return (state == ifile::IFilePersistence::OS_OK);
}


bool CLicenseControllerComp::CheckLicense(const QByteArray& key, const QString& licensePath) const
{
	if (!m_productInstancePersistenceCompPtr.IsValid()){
		SendCriticalMessage(0, QT_TR_NOOP("No product instance persistence component was set. Please check component configuration"), "CLicenseControllerComp::CheckLicense");

		return false;
	}

	if (!m_encryptedInstancePersistenceCompPtr.IsValid()){
		SendCriticalMessage(0, QT_TR_NOOP("No encrypted instance persistence component was set. Please check component configuration"), "CLicenseControllerComp::CheckLicense");

		return false;
	}

	if (licensePath.isEmpty()){
		SendWarningMessage(0, QT_TR_NOOP("License file path is empty, nothing to check"), "CLicenseControllerComp::CheckLicense");
		return false;
	}

	QFileInfo licenseFileInfo(licensePath);
	if (!licenseFileInfo.exists()){
		SendWarningMessage(0, QT_TR_NOOP(QString("License file '%1' doesn't exist").arg(licensePath)), "CLicenseControllerComp::CheckLicense");
		return false;
	}

	imtlic::CProductInstanceInfo tempLicense;
	int state = m_encryptedInstancePersistenceCompPtr->LoadFromEncryptedFile(key, licensePath, tempLicense);

	return (state == ifile::IFilePersistence::OS_OK);
}


// protected methods

void CLicenseControllerComp::OnFingeprintCheckTimer() const
{
	SendVerboseMessage(QT_TR_NOOP("Checking license fingerprint"), tr("License Controller"));

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

	constexpr std::chrono::hours oneHour(1);
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
	m_licenseStatus.SetGoodwillRemainingDays(0);

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
		SendErrorMessage(0, QT_TR_NOOP(QString("License file '%1' doesn't exist").arg(licenseFilePath)), "License Management");

		m_licenseStatus.SetLicenseStatusFlags(imtlic::ILicenseStatus::LSF_LICENSE_INVALID | imtlic::ILicenseStatus::LSF_NO_LICENSE);

		return false;
	}

	QString fingerprintFilePath = GetFingerprintPath();

	int state = m_productInstancePersistenceCompPtr->LoadFromFile(license, licenseFilePath);
	if (state != ifile::IFilePersistence::OS_OK){
		int daysUntilExpire = -1;
		if (LoadFingerprint(fingerprintFilePath, license, daysUntilExpire)){
			QByteArray publicKey;
			if (m_licenseKeysProviderCompPtr.IsValid()){
				publicKey = m_licenseKeysProviderCompPtr->GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KT_PASSWORD);
			}
			if (!publicKey.contains("[Not Available]")){
				SendWarningMessage(0, QT_TR_NOOP(QString("You have no valid license to run this software anymore. You have %1 day(s) to update your system with a valid license").arg(daysUntilExpire)), "License Management");
			}
			else{
				SendWarningMessage(0, QT_TR_NOOP(QString("Sensor is not connected. You have %1 day(s) for running the software without the sensor.").arg(daysUntilExpire)), "License Management");
			}
			m_licenseStatus.SetLicenseStatusFlags(imtlic::ILicenseStatus::LSF_LICENSE_INVALID | imtlic::ILicenseStatus::LSF_GOODWILL);

			m_licenseStatus.SetGoodwillRemainingDays(daysUntilExpire);
		}
		else{
			SendErrorMessage(0, QT_TR_NOOP(QString("You have no license to run this software. License file: '%1'").arg(licenseFilePath)), "License Management");

			return false;
		}
	}
	else{
		if (!UpdateFingerprint(fingerprintFilePath, license)){
			SendWarningMessage(0, QT_TR_NOOP("License fingerprint could not be updated"), "License Management");
		}

		SendVerboseMessage(QT_TR_NOOP("License was successfully loaded"), "License Management");

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

		int days = imtbase::narrow_cast<int>(fingerprintTimeStamp.daysTo(currentTime));
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

	quint32 checksum = istd::CCrcCalculator::GetCrcFromFile(licenseFilePath);

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
			SendVerboseMessage(QT_TR_NOOP("License key was changed. Checking license fingerprint"), tr("License Controller"));

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
	:m_licenseStatusFlags(LSF_LICENSE_INVALID),
	m_goodwillRemainingDays(0)
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


int CLicenseControllerComp::LicenseStatus::GetGoodwillRemainingDays() const
{
	return m_goodwillRemainingDays;
}


void CLicenseControllerComp::LicenseStatus::SetGoodwillRemainingDays(int days)
{
	if (m_goodwillRemainingDays != days){
		istd::CChangeNotifier changeNotifier(this);

		m_goodwillRemainingDays = days;
	}
}


} // namespace imtlic


