#include <imtservergql/CGqlSearchControllerComp.h>


// ACF includes
#include <istd/TDelPtr.h>


namespace imtservergql
{


// protected methods

// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CGqlSearchControllerComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_optionListCompPtr.IsValid()){
		errorMessage = QString("CGqlSearchControllerComp attribute 'm_optionListCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "imtgql::CGqlSearchControllerComp");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("GraphQL input params is invalid");
		SendErrorMessage(0, errorMessage, "imtgql::CGqlSearchControllerComp");

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

					for (int resultIndex = 0; resultIndex < resultsCount; ++resultIndex){
						int itemIndex = itemsModelPtr->InsertNewItem();

						imtbase::ISearchResults::SearchResult searchResult = searchResultsPtr->GetSearchResult(resultIndex);
						if (searchResult.resultName.isEmpty()){
							searchResult.resultName = QT_TR_NOOP("Unnamed result");
						}

						itemsModelPtr->SetData("Id", searchResult.contextId, itemIndex);
						itemsModelPtr->SetData("TypeId", searchResult.contextTypeId, itemIndex);
						itemsModelPtr->SetData("Name", searchResult.resultName, itemIndex);
						itemsModelPtr->SetData("Description", searchResult.resultDescription, itemIndex);
					}
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtservergql


