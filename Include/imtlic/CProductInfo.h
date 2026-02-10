// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtbase/CObjectCollection.h>
#include <imtlic/IProductInfo.h>


namespace imtlic
{


class CProductInfo: virtual public IProductInfo
{
public:
	CProductInfo();

	// reimplemented (IProductInfo)
	virtual const IProductInfo* GetProductFamilyInfo() const override;
	virtual QByteArray GetProductId() const override;
	virtual void SetProductId(const QByteArray& productId) override;
	virtual QString GetProductDescription() const override;
	virtual void SetProductDescription(const QString& description) override;
	virtual QByteArray GetCategoryId() const override;
	virtual void SetCategoryId(const QByteArray& categoryId) override;
	virtual imtbase::IObjectCollection* GetFeatures() override;
	virtual bool AddFeature(const QByteArray& featureId, const IFeatureInfo& featureInfo) override;
	virtual bool RemoveFeature(const QByteArray& featureId) override;

	// reimplemented (iprm::INameParam)
	virtual const QString& GetName() const override;
	virtual void SetName(const QString& name) override;
	virtual bool IsNameFixed() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_productId;
	QString m_productName;
	QString m_productDescription;
	QByteArray m_categoryId;

	imtbase::CObjectCollection m_featureCollection;
	const imtlic::IFeatureInfoProvider* m_featureInfoProviderPtr;
};


typedef imtbase::TIdentifiableWrap<CProductInfo> CIdentifiableProductInfo;


} // namespace imtlic


