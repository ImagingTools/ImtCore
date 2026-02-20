// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CHeadersDataProviderComp.h>


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

imtbase::CTreeItemModel* CHeadersDataProviderComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if(gqlContextPtr != nullptr){
		languageId =  gqlContextPtr->GetLanguageId();
	}

	if (gqlContextPtr == nullptr){
		errorMessage = QString("Unable to create response. Error: GraphQL context is invalid");
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> dataModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* headersModelPtr = dataModelPtr->AddTreeModel("headers");

	const int idsCount = m_headersIdsAttrPtr.GetCount();
	const int namesCount = m_headersNamesAttrPtr.GetCount();
	Q_ASSERT_X(idsCount == namesCount, __func__, "Not every header ID has an assigned name.");
	for (int i = 0; i < qMin(idsCount, namesCount); i++){
		QString headerId = m_headersIdsAttrPtr[i];

		if (!CheckHeaderPermission(headerId.toUtf8(), *gqlContextPtr)){
			continue;
		}

		int index = headersModelPtr->InsertNewItem();

		headersModelPtr->SetData("id", m_headersIdsAttrPtr[i], index);

		QString headerName = m_headersNamesAttrPtr[i];

		if (m_translationManagerCompPtr.IsValid()){
			QByteArray context = "Attribute";
			QString headerNameTr = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), headerName.toUtf8(), languageId, context);
			headerName = headerNameTr;
		}

		headersModelPtr->SetData("name", headerName, index);
	}

	if (m_headersSearchByFilterAttrPtr.IsValid()){
		imtbase::CTreeItemModel* searchModel = dataModelPtr->AddTreeModel("filterSearch");
		for (int i = 0; i < m_headersSearchByFilterAttrPtr.GetCount(); i++){
			QString headerId = m_headersSearchByFilterAttrPtr[i];
			if (!CheckHeaderPermission(headerId.toUtf8(), *gqlContextPtr)){
				continue;
			}

			int index = searchModel->InsertNewItem();
			searchModel->SetData("id", m_headersSearchByFilterAttrPtr[i], index);
		}
	}

	return dataModelPtr.PopPtr();
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


