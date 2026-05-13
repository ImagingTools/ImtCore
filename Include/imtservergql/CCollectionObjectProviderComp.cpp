// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CCollectionObjectProviderComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ACF includes
#include <iser/CJsonMemWriteArchive.h>


namespace imtservergql
{


// reimplemented (imtgql::CCGqlRepresentationControllerCompBase)

QJsonObject CCollectionObjectProviderComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CCollectionObjectProviderComp");
		return QJsonObject();
	}

	QByteArray objectId;

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetParamArgumentValue("Id").toByteArray();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		iser::IObject* objectInfoPtr = dynamic_cast<iser::IObject*>(dataPtr.GetPtr());
		Q_ASSERT_X(objectInfoPtr != nullptr, __FILE__, "iser::IObject interface not supported");
		if (objectInfoPtr == nullptr){
			errorMessage = QString("INTERNAL: Object implementation doesn't support iser::IObject interface").toUtf8();

			SendErrorMessage(0, errorMessage);

			return QJsonObject();
		}

		QByteArray json;
		{
			iser::CJsonMemWriteArchive archive;
			if (!objectInfoPtr->Serialize(archive)){
				errorMessage = QString("Object serialization failed").toUtf8();

				SendErrorMessage(0, errorMessage);

				return QJsonObject();
			}

			json = archive.GetData();
		}

		QJsonDocument doc = QJsonDocument::fromJson(json);
		if (doc.isNull() || !doc.isObject()){
			errorMessage = QString("JSON object creation from serialized data failed").toUtf8();
			SendErrorMessage(0, errorMessage);

			return QJsonObject();
		}

		QJsonObject rootObj;
		rootObj.insert(QStringLiteral("data"), doc.object());
		return rootObj;
	}
	else{
		errorMessage = QString("Object with the ID '%1' doesn't exist").arg(qPrintable(objectId)).toUtf8();
		SendErrorMessage(0, errorMessage);

		return QJsonObject();
	}
}


} // namespace imtservergql


