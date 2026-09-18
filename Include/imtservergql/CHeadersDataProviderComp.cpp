// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CHeadersDataProviderComp.h>

// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

// ACF includes
#include <iqt/iqt.h>


namespace imtservergql
{


// protected methods

bool CHeadersDataProviderComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (!isSupported){
		QByteArray commandId = gqlRequest.GetCommandId();

		return commandId.isEmpty();
	}

	return true;
}


// reimplemented (imtservergql::CGqlRepresentationControllerCompBase)

QJsonObject CHeadersDataProviderComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if(gqlContextPtr != nullptr){
		languageId =  gqlContextPtr->GetLanguageId();
	}

	if (gqlContextPtr == nullptr){
		errorMessage = QStringLiteral("Unable to create response. Error: GraphQL context is invalid");
		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonArray headersArray;

	const int idsCount = m_headersIdsAttrPtr.GetCount();
	const int namesCount = m_headersNamesAttrPtr.GetCount();
	Q_ASSERT_X(idsCount == namesCount, __func__, "Not every header ID has an assigned name.");
	for (int i = 0; i < qMin(idsCount, namesCount); i++){
		QString headerId = m_headersIdsAttrPtr[i];

		if (!CheckHeaderPermission(headerId.toUtf8(), *gqlContextPtr)){
			continue;
		}

		QJsonObject headerObj;
		headerObj.insert(QStringLiteral("id"), m_headersIdsAttrPtr[i]);

		QString headerName = m_headersNamesAttrPtr[i];

		if (m_translationManagerCompPtr.IsValid()){
			QByteArray context = "Attribute";
			QString headerNameTr = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), headerName.toUtf8(), languageId, context);
			headerName = headerNameTr;
		}

		headerObj.insert(QStringLiteral("name"), headerName);
		headersArray.append(headerObj);
	}

	rootObj.insert(QStringLiteral("headers"), headersArray);

	if (m_headersSearchByFilterAttrPtr.IsValid()){
		QJsonArray searchArray;
		for (int i = 0; i < m_headersSearchByFilterAttrPtr.GetCount(); i++){
			QString headerId = m_headersSearchByFilterAttrPtr[i];
			if (!CheckHeaderPermission(headerId.toUtf8(), *gqlContextPtr)){
				continue;
			}

			QJsonObject searchObj;
			searchObj.insert(QStringLiteral("id"), m_headersSearchByFilterAttrPtr[i]);
			searchArray.append(searchObj);
		}
		rootObj.insert(QStringLiteral("filterSearch"), searchArray);
	}

	return rootObj;
}


// private methods

bool CHeadersDataProviderComp::CheckHeaderPermission(const QByteArray& headerId, const imtgql::IGqlContext& gqlContext) const
{
	if (m_headerPermissionsAttrPtr.GetCount() == 0){
		return true;
	}

	const imtauth::IUserInfo* userInfoPtr = gqlContext.GetUserInfo();
	if (userInfoPtr == nullptr){
		return false;
	}

	if (userInfoPtr->IsAdmin()){
		return true;
	}

	int index = m_headersIdsAttrPtr.FindValue(headerId);
	if (index < 0){
		return false;
	}

	if (m_headerPermissionsAttrPtr.GetCount() <= index){
		return false;
	}

	QByteArrayList userPermissions = userInfoPtr->GetPermissions();
	QByteArray headerPermission = m_headerPermissionsAttrPtr[index];

	if (headerPermission.isEmpty()){
		return true;
	}

	return userPermissions.contains(headerPermission);
}


} // namespace imtservergql


