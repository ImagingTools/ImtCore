// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CProductMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtlic/IProductInfo.h>


namespace imtlic
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CProductMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(dataPtr);
	if (productInfoPtr == nullptr){
		return false;
	}
	
	metaInfoPtr->SetMetaInfo(IProductInfo::MIT_PRODUCT_NAME, productInfoPtr->GetName());
	metaInfoPtr->SetMetaInfo(IProductInfo::MIT_PRODUCT_ID, productInfoPtr->GetProductId());
	metaInfoPtr->SetMetaInfo(IProductInfo::MIT_PRODUCT_CATEGORY, productInfoPtr->GetCategoryId());
	metaInfoPtr->SetMetaInfo(IProductInfo::MIT_PRODUCT_DESCRIPTION, productInfoPtr->GetProductDescription());

	return true;
}


// public methods of embedded class MetaInfo

QString CProductMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case IProductInfo::MIT_PRODUCT_NAME:
		return QObject::tr("Product Name");
	case IProductInfo::MIT_PRODUCT_ID:
		return QObject::tr("Product-ID");
	case IProductInfo::MIT_PRODUCT_CATEGORY:
		return QObject::tr("Category");
	case IProductInfo::MIT_PRODUCT_DESCRIPTION:
		return QObject::tr("Description");
	}
	
	return BaseClass::GetMetaInfoName(metaInfoType);
}



} // namespace imtlic


