#pragma once


// Qt includes
#include <QtCore/QTimer>

// ACF includes
#include <imod/IModel.h>
#include <imod/TModelWrap.h>
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtcrypt/IEncryptionKeysProvider.h>
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/ILicenseController.h>
#include <imtlic/ILicenseStatus.h>


namespace imtlic
{


/**
	Client-side license check controller.
	\ingroup LicenseManagement
*/
class CLicenseControllerComp: public QObject, public ilog::CLoggerComponentBase, virtual public ILicenseController
{
	Q_OBJECT

public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLicenseControllerComp);
		I_REGISTER_INTERFACE(ILicenseController);
		I_REGISTER_SUBELEMENT(LicenseStatus);
		I_REGISTER_SUBELEMENT_INTERFACE(LicenseStatus, imtlic::ILicenseStatus, ExtractLicenseStatus);
		I_REGISTER_SUBELEMENT_INTERFACE(LicenseStatus, istd::IChangeable, ExtractLicenseStatus);
		I_REGISTER_SUBELEMENT_INTERFACE(LicenseStatus, imod::IModel, ExtractLicenseStatus);
		I_ASSIGN(m_productInstanceCompPtr, "ProductInstance", "Instance of the product installation", true, "ProductInstance");
		I_ASSIGN(m_productInstancePersistenceCompPtr, "ProductInstancePersistence", "Encrypted ersistence for the product instance", true, "ProductInstancePersistence");
		I_ASSIGN(m_fingerprintInstancePersistenceCompPtr, "FingerprintInstancePersistence", "Fingerprint persistence for the product instance", true, "FingerprintPersistence");
		I_ASSIGN(m_licensePathCompPtr, "LicenseFilePath", "Path to the license file", true, "LicenseFilePath");
		I_ASSIGN(m_licenseKeysProviderCompPtr, "LicenseKeys", "Provider of license keys", false, "LicenseKeys");
		I_ASSIGN(m_licenseFingerprintPathCompPtr, "LicenseFingerprintPath", "Folder containing license fingerprint file", false, "LicenseFingerprintPath");
		I_ASSIGN(m_fingerprintExpirationAttrPtr, "FingerprintExpiration", "Time in days for the license fingerprint expiration", false, 5);
	I_END_COMPONENT;

	CLicenseControllerComp();

	// reimplemented (ILicenseController)
	virtual bool ImportLicense(const QString& licenseFilePath, ilog::IMessageConsumer* logPtr = nullptr) const override;

protected:
	void OnFingeprintCheckTimer() const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	bool ReadLicenseFromFile(imtlic::IProductInstanceInfo& license) const;
	bool UpdateFingerprint(const QString& filePath, imtlic::IProductInstanceInfo& license) const;
	bool LoadFingerprint(const QString& filePath, imtlic::IProductInstanceInfo& license, int& daysUntilExpire) const;
	QString GetFingerprintPath() const;
	void OnLicenseKeysUpdated(const istd::IChangeable::ChangeSet& changeSet, const imtcrypt::IEncryptionKeysProvider* licenseKeysProviderPtr);

private:
	class LicenseStatus: public imtlic::ILicenseStatus
	{
	public:
		LicenseStatus();

		// reimplemented (imtlic::ILicenseStatus)
		virtual int GetLicenseStatusFlags() const override;
		virtual void SetLicenseStatusFlags(int licenseStatusFlags) override;
		virtual QString GetLicenseLocation() const override;
		virtual void SetLicenseLocation(const QString & licenseLocation) override;

	private:
		int m_licenseStatusFlags;
		QString m_licenseLocation;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractLicenseStatus(CLicenseControllerComp& component)
	{
		return &component.m_licenseStatus;
	}

private:
	imtbase::TModelUpdateBinder<imtcrypt::IEncryptionKeysProvider, CLicenseControllerComp> m_licenseKeysProvider;
	bool m_isInitializing;

	I_REF(imtlic::IProductInstanceInfo, m_productInstanceCompPtr);
	I_REF(ifile::IFilePersistence, m_fingerprintInstancePersistenceCompPtr);
	I_REF(ifile::IFilePersistence, m_productInstancePersistenceCompPtr);
	I_REF(ifile::IFileNameParam, m_licensePathCompPtr);
	I_REF(ifile::IFileNameParam, m_licenseFingerprintPathCompPtr);
	I_REF(imtcrypt::IEncryptionKeysProvider, m_licenseKeysProviderCompPtr);
	I_ATTR(int, m_fingerprintExpirationAttrPtr);

	QTimer m_checkLicenseTimer;

	mutable imod::TModelWrap<LicenseStatus> m_licenseStatus;
};


} // namespace imtlic


