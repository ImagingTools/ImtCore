#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtbase/CCollectionInfo.h>
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/CLicenseInstance.h>


namespace imtlic
{


/**
	Common implementation of IProductInstanceInfo interface.
	\sa IProductInstanceInfo
	\ingroup LicenseManagement
*/
class CProductInstanceInfo: virtual public imtlic::IProductInstanceInfo
{
public:
	CProductInstanceInfo();

	// reimplemented (imtlic::IProductInstanceInfo)
	virtual const imtbase::IObjectCollection* GetProductDatabase() const override;
	virtual const imtbase::IObjectCollection* GetCustomerDatabase() const override;
	virtual void SetupProductInstance(
				const QByteArray& productId,
				const QByteArray& instanceId,
				const QByteArray& customerId) override;
	virtual void AddLicense(const QByteArray& licenseId, const QDateTime& expirationDate = QDateTime()) override;
	virtual void RemoveLicense(const QByteArray& licenseId) override;
	virtual void ClearLicenses() override;
	virtual QByteArray GetProductId() const override;
	virtual QByteArray GetProductInstanceId() const override;
	virtual void SetProductInstanceId(const QByteArray& instanceId) override;
	virtual QByteArray GetCustomerId() const override;
	virtual QByteArray GetSerialNumber() const override;
	virtual void SetSerialNumber(const QByteArray& serialNumber) override;
	virtual QByteArray GetProject() const override;
	virtual void SetProject(const QByteArray& project) override;
	virtual bool IsInUse() const override;
	virtual void SetInUse(bool inUse) override;

	// reimplemented (imtlic::ILicenseInfoProvider)
	virtual const imtbase::ICollectionInfo& GetLicenseInstances() const override;
	virtual const imtlic::ILicenseInstance* GetLicenseInstance(const QByteArray& licenseId) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual QByteArray GetFactoryId() const override;

protected:
	const imtbase::IObjectCollection* m_customerCollectionPtr;
	const imtbase::IObjectCollection* m_productCollectionPtr;

private:
	typedef istd::TSmartPtr<CLicenseInstance> LicenseInstancePtr;
	typedef QMap<QByteArray /*ID of the license definition in the product*/, LicenseInstancePtr> LicenseInstances;

	QByteArray m_productId;
	QByteArray m_customerId;
	QByteArray m_instanceId;
	QByteArray m_serialNumber;
	QByteArray m_project;
	bool m_inUse;
	LicenseInstances m_licenses;
	imtbase::CCollectionInfo m_licenseContainerInfo;
};


typedef imtbase::TIdentifiableWrap<CProductInstanceInfo> CIdentifiableSoftwareInstanceInfo;


} // namespace imtlic


