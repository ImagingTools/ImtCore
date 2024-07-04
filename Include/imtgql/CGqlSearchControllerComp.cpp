#include <imtgql/CGqlSearchControllerComp.h>


// ACF includes
#include <istd/TDelPtr.h>


namespace imtgql
{


// protected methods

// reimplemented (imtgql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CGqlSearchControllerComp::CreateInternalResponse(const CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		return nullptr;
	}

	if (!m_optionListCompPtr.IsValid()){
		return nullptr;
	}

	QString text = inputParamPtr->GetFieldArgumentValue("Text").toString();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	for (int i = 0; i < m_searchControllersCompPtr.GetCount(); i++){
		imtbase::ISearchController* searchControllerPtr = m_searchControllersCompPtr[i];
		if (searchControllerPtr != nullptr){
			QByteArray controllerId = m_optionListCompPtr->GetOptionId(i);
			QString name = m_optionListCompPtr->GetOptionName(i);

			istd::TDelPtr<const imtbase::ISearchResults> searchResultsPtr(searchControllerPtr->Search(text));
			if (searchResultsPtr.IsValid()){
				int resultsCount = searchResultsPtr->GetSearchResultsCount();
				if (resultsCount > 0){
					int index = dataModelPtr->InsertNewItem();
					dataModelPtr->SetData("Id", controllerId, index);
					dataModelPtr->SetData("Name", name, index);

					imtbase::CTreeItemModel* itemsModelPtr = dataModelPtr->AddTreeModel("Items", index);

					for (int i = 0; i < resultsCount; i++){
						int itemIndex = itemsModelPtr->InsertNewItem();

						imtbase::ISearchResults::SearchResult searchResult = searchResultsPtr->GetSearchResult(i);

						itemsModelPtr->SetData("Id", searchResult.contextId, itemIndex);
						itemsModelPtr->SetData("Name", searchResult.resultName, itemIndex);
						itemsModelPtr->SetData("Description", searchResult.resultDescription, itemIndex);
					}
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtgql


