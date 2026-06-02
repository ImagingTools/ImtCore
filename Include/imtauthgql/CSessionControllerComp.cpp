// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>
#include <imtauthgql/CSessionControllerComp.h>


// Qt includes
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtauth/ISession.h>


namespace imtauthgql
{


// protected methods

// reimplemented (CObjectCollectionControllerCompBase)

QJsonObject CSessionControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribure 'ObjectCollection' was not set", "CSessionControllerComp");
		return QJsonObject();
	}

	QJsonObject dataObj;

	QByteArray sessionId = GetObjectIdFromInputParams(gqlRequest.GetParams());

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(sessionId, dataPtr)){
		auto sessionInfoPtr = dynamic_cast<const imtauth::ISession*>(dataPtr.GetPtr());
		if (sessionInfoPtr != nullptr){
			QByteArray token = sessionInfoPtr->GetToken();
			QByteArray userId = sessionInfoPtr->GetUserId();

			dataObj.insert("UserId", QString::fromUtf8(userId));
			dataObj.insert("Token", QString::fromUtf8(token));
		}
	}

	QJsonObject rootObj;
	rootObj.insert("data", dataObj);

	return rootObj;
}


} // namespace imtauthgql

