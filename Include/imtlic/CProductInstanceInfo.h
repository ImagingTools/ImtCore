#pragma once


// ImtCore includes
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
				const QString& productId,
				const QByteArray& instanceId,
				const QString& customerId) override;
	virtual void AddLicense(const QByteArray& licenseId, const QDateTime& expirationDate = QDateTime()) override;
	virtual void RemoveLicense(const QByteArray& licenseId) override;
	virtual void ClearLicenses() override;
	virtual QString GetProductId() const override;
	virtual QByteArray GetProductInstanceId() const override;
	virtual QString GetCustomerId() const override;

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

protected:
	const imtbase::IObjectCollection* m_customerCollectionPtr;
	const imtbase::IObjectCollection* m_productCollectionPtr;

private:
	QByteArray GetProductId(const QString& productName) const;

private:
	typedef istd::TSmartPtr<CLicenseInstance> LicenseInstancePtr;
	typedef QMap<QByteArray /*ID of the license definition in the product*/, LicenseInstancePtr> LicenseInstances;

	QString m_productId;
	QString m_customerId;
	QByteArray m_instanceId;
	LicenseInstances m_licenses;
	imtbase::CCollectionInfo m_licenseContainerInfo;
};


} // namespace imtlic


