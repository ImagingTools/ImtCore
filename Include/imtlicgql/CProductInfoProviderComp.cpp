// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CProductInfoProviderComp.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>


namespace imtlicgql
{


// protected methods

// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

bool CProductInfoProviderComp::IsRequestSupported(const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	return true;
}


QJsonObject CProductInfoProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QJsonObject rootObj;
	QJsonArray productsArray;

	for (int i = 0; i < m_productIdsAttrPtr.GetCount(); i++){
		QJsonObject productObj;

		productObj.insert(QStringLiteral("id"), QJsonValue::fromVariant(m_productIdsAttrPtr[i]));
		productObj.insert(QStringLiteral("name"), QJsonValue::fromVariant(m_productNamesAttrPtr[i]));

		imtgql::IGqlRequestHandler* representationDataProvider = m_permissionsProviderCompPtr[i];
		if (representationDataProvider != nullptr){
			QJsonObject permissionsObj = representationDataProvider->CreateResponse(gqlRequest, errorMessage);
			if (!permissionsObj.isEmpty()){
				productObj.insert(QStringLiteral("permissions"), permissionsObj);
			}
		}

		productsArray.append(productObj);
	}

	rootObj.insert(QStringLiteral("data"), productsArray);
	return rootObj;
}


} // namespace imtlicgql


