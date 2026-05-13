// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CProductPermissionsControllerComp.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>


namespace imtlicgql
{


// public methods

// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

QJsonObject CProductPermissionsControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_productProviderCompPtr.IsValid()){
		SendErrorMessage(0, QString("Internal error."), "CProductPermissionsControllerComp");

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		SendErrorMessage(0, QString("Unable to create object. GQL input params is invalid."), "CProductPermissionsControllerComp");

		return QJsonObject();
	}

	QByteArray productId = inputParamPtr->GetParamArgumentValue("productId").toByteArray();
	if (productId.isEmpty()){
		SendErrorMessage(0, QString("Unable to get permission for product with empty ID."), "CProductPermissionsControllerComp");

		return QJsonObject();
	}

	QJsonObject rootObj;

	QJsonObject productsObj = m_productProviderCompPtr->CreateResponse(gqlRequest, errorMessage);
	if (!productsObj.isEmpty()){
		QJsonArray productsArray = productsObj.value(QStringLiteral("data")).toArray();
		for (int i = 0; i < productsArray.size(); i++){
			QJsonObject productObj = productsArray[i].toObject();
			QByteArray currentProductId = productObj.value(QStringLiteral("id")).toString().toUtf8();
			if (currentProductId == productId){
				QJsonValue permissionsValue = productObj.value(QStringLiteral("permissions"));
				if (!permissionsValue.isNull() && !permissionsValue.isUndefined()){
					QJsonObject permissionsObj = permissionsValue.toObject();
					QJsonValue permissionsData = permissionsObj.value(QStringLiteral("data"));
					if (!permissionsData.isNull() && !permissionsData.isUndefined()){
						rootObj.insert(QStringLiteral("data"), permissionsData);
					}
				}
			}
		}
	}

	return rootObj;
}


} // namespace imtlicgql


