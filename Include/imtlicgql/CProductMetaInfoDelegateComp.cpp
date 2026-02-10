// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CProductMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ImtCore includes
#include <imtlic/IProductInfo.h>


namespace imtlicgql
{


// protected methods

bool CProductMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray productId = metaInfo.GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_ID).toByteArray();
	representation["ProductId"] = QString(productId);
	
	QString name = metaInfo.GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_NAME).toString();
	representation["ProductName"] = QString(name);
	
	QString description = metaInfo.GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_DESCRIPTION).toString();
	representation["Description"] = QString(description);
	
	QByteArray category = metaInfo.GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_CATEGORY).toByteArray();
	representation["Category"] = QString(category);

	return true;
}


bool CProductMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation) const
{
	if (representation.contains("ProductId")){
		metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_ID, representation.value("ProductId"));
	}
	
	if (representation.contains("ProductName")){
		metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_NAME, representation.value("ProductName"));
	}
	
	if (representation.contains("Description")){
		metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_DESCRIPTION, representation.value("Description"));
	}
	
	if (representation.contains("Category")){
		metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_CATEGORY, representation.value("Category"));
	}
	
	return true;
}


} // namespace imtlicgql


