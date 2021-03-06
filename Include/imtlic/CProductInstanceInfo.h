#pragma once


// ImtCore includes
#include <imtbase/CSimpleReferenceCollection.h>
#include <imtlic/IProductInstanceInfo.h>


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
	virtual void AddLicense(const QByteArray& licenseId) override;
	virtual void RemoveLicense(const QByteArray& licenseId) override;
	virtual QByteArray GetProductId() const override;
	virtual QByteArray GetProductInstanceId() const override;
	virtual QByteArray GetCustomerId() const override;

	// reimplemented (imtlic::ILicenseInfoProvider)
	virtual const imtbase::ICollectionInfo& GetLicenseList() const override;
	virtual const imtlic::ILicenseInfo* GetLicenseInfo(const QByteArray& licenseId) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	imtbase::CSimpleReferenceCollection m_licenses;
	QByteArray m_productId;
	QByteArray m_customerId;
	QByteArray m_instanceId;

	const imtbase::IObjectCollection* m_customerCollectionPtr;
	const imtbase::IObjectCollection* m_productCollectionPtr;
};


} // namespace imtlic


