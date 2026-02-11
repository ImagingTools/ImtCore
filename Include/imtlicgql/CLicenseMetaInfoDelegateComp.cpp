// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CLicenseMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ImtCore includes
#include <imtlic/ILicenseDefinition.h>


namespace imtlicgql
{


// protected methods

bool CLicenseMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray licenseId = metaInfo.GetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_ID).toByteArray();
	representation["LicenseId"] = QString(licenseId);
	
	QString licenseName = metaInfo.GetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_NAME).toString();
	representation["LicenseName"] = QString(licenseName);
	
	QString description = metaInfo.GetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_DESCRIPTION).toString();
	representation["Description"] = QString(description);
	
	QByteArray productId = metaInfo.GetMetaInfo(imtlic::ILicenseDefinition::MIT_PRODUCT_ID).toByteArray();
	representation["ProductId"] = QString(productId);
	
	return true;
}


bool CLicenseMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation) const
{
	if (representation.contains("LicenseId")){
		metaInfo.SetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_ID, representation.value("LicenseId"));
	}
	
	if (representation.contains("LicenseName")){
		metaInfo.SetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_NAME, representation.value("LicenseName"));
	}
	
	if (representation.contains("Description")){
		metaInfo.SetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_DESCRIPTION, representation.value("Description"));
	}
	
	if (representation.contains("ProductId")){
		metaInfo.SetMetaInfo(imtlic::ILicenseDefinition::MIT_PRODUCT_ID, representation.value("ProductId"));
	}
	
	return true;
}


} // namespace imtlicgql


