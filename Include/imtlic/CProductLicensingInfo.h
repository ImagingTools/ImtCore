#pragma once


// ImtCore includes
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/CLicenseInfoManager.h>


namespace imtlic
{


/**
	Common implementation of IProductLicensingInfo interface.
	\sa ILicenseInfo
	\ingroup LicenseManagement
*/
class CProductLicensingInfo: 
			virtual public imtlic::IProductLicensingInfo,
			public CLicenseInfoManager
{
public:
	typedef CLicenseInfoManager BaseClass;

	CProductLicensingInfo();

	// reimplemented (imtlic::IProductLicensingInfo)
	virtual const imtbase::ICollectionInfo* GetProductList() const override;

	// reimplemented (imtlic::IProductInfo)
	virtual const IProductInfo* GetProductFamilyInfo() const override;
	virtual QByteArray GetProductId() const override;
	virtual void SetProductId(const QByteArray& productId) override;

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
	imtbase::ICollectionInfo* m_collectionInfoPtr;
	QString m_productName;
	QByteArray m_productId;
};


} // namespace imtlic


