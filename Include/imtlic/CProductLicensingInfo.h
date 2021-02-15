#pragma once


// ImtCore includes
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/CLicenseInfoManager.h>


namespace imtlic
{


/**
	Information about the available licenses of a product.
	\sa ILicenseInfo
	\ingroup LicenseManagement
*/
class CProductLicensingInfo: virtual public imtlic::IProductLicensingInfo
{
public:
	// reimplemented (imtlic::IProductInfo)
	virtual const IProductInfo* GetProductFamilyInfo() const override;
	virtual QByteArray GetProductId() const override;

	// reimplemented (imtlic::ILicenseInfoProvider)
	virtual const imtbase::ICollectionInfo& GetLicenseList() const override;
	virtual const imtlic::ILicenseInfo* GetLicenseInfo(const QByteArray& licenseId) const override;

	// reimplemented (iprm::INameParam)
	virtual const QString& GetName() const override;
	virtual void SetName(const QString& name) override;
	virtual bool IsNameFixed() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	CLicenseInfoManager m_licenses;
	QString m_productName;
	QByteArray m_productId;
};


} // namespace imtlic


