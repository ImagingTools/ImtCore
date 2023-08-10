#include <imtqml/CHeadersDataProviderComp.h>


// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtqml
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

bool CHeadersDataProviderComp::IsRequestSupported(const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	return true;
}


imtbase::CTreeItemModel* CHeadersDataProviderComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> dataModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* headersModelPtr = dataModelPtr->AddTreeModel("Headers");

	imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();

	QByteArray languageId;
	if(gqlContextPtr != nullptr){
		languageId =  gqlContextPtr->GetLanguageId();
	}

	for (int i = 0; i < m_headersIdsAttrPtr.GetCount(); i++){
		headersModelPtr->InsertNewItem();

		headersModelPtr->SetData("Id", m_headersIdsAttrPtr[i], i);

		QString headerName = m_headersNamesAttrPtr[i];

		if (m_translationManagerCompPtr.IsValid()){
			QByteArray context = "Attribute";
			QString headerNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), headerName.toUtf8(), languageId, context);
			headerName = headerNameTr;
		}

		headersModelPtr->SetData("Name", headerName, i);
	}

	if (m_headersSearchByFilterAttrPtr.IsValid()){
		imtbase::CTreeItemModel* searchModel = dataModelPtr->AddTreeModel("FilterSearch");
		for (int i = 0; i < m_headersSearchByFilterAttrPtr.GetCount(); i++){
			int index = searchModel->InsertNewItem();
			searchModel->SetData("Id", m_headersSearchByFilterAttrPtr[i], index);
		}
	}

	return dataModelPtr.PopPtr();
}


} // namespace imtqml


