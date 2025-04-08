#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CSearchResults.h>
#include <imtbase/CTreeItemModel.h>
#include <imtbase/ISearchController.h>
#include <imtgql/IGqlRequestProvider.h>
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilter.h>

#undef GetObject


namespace imtservergql
{


template <class CollectionController>
class TObjectCollectionControllerSearchCompWrap:
			public CollectionController,
			virtual public imtbase::ISearchController
{
public:
	typedef CollectionController BaseClass;

	I_BEGIN_COMPONENT(TObjectCollectionControllerSearchCompWrap);
		I_REGISTER_INTERFACE(imtbase::ISearchController);
		I_ASSIGN(m_listObjectCommandAttrPtr, "ListObjectCommand", "The command-ID for getting a list of objects", false, "");
	I_END_COMPONENT;

	// reimplemented (ISearchController)
	virtual QByteArray GetControllerId() const override;
	virtual const imtbase::ISearchResults* Search(const QString& text) const override;

protected:
	I_ATTR(QByteArray, m_listObjectCommandAttrPtr);
};


// public methods

template<class ObjectCollection>
QByteArray TObjectCollectionControllerSearchCompWrap<ObjectCollection>::GetControllerId() const
{
	return QByteArray();
}


template<class CollectionController>
const imtbase::ISearchResults* TObjectCollectionControllerSearchCompWrap<CollectionController>::Search(const QString& text) const
{
	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_QUERY, m_listObjectCommandAttrPtr.IsValid() ? m_listObjectCommandAttrPtr->GetValue() : QByteArray());

	imtgql::IGqlRequestProvider* gqlRequestProviderPtr = QueryInterface<imtgql::IGqlRequestProvider>(const_cast<TObjectCollectionControllerSearchCompWrap<CollectionController>*>(this));
	if (gqlRequestProviderPtr != nullptr){
		const imtgql::IGqlRequest* gqlRequestPtr = gqlRequestProviderPtr->GetGqlRequest();
		if (gqlRequestPtr != nullptr){
			istd::TDelPtr<imtgql::IGqlContext> gqlContextPtr;
			gqlContextPtr.SetCastedOrRemove(gqlRequestPtr->GetRequestContext()->CloneMe());
			if (!gqlContextPtr.IsValid()){
				return nullptr;
			}

			gqlRequest.SetGqlContext(gqlContextPtr.PopPtr());
		}
	}

	QString errorMessage;
	bool ok = BaseClass::CheckPermissions(gqlRequest, errorMessage);
	if (!ok){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> headersResponseModelPtr = BaseClass::GetHeaders(gqlRequest, errorMessage);
	if (!headersResponseModelPtr.IsValid()){
		return nullptr;
	}

	imtbase::CTreeItemModel* headersDataModelPtr = headersResponseModelPtr->GetTreeItemModel("data");
	if (headersDataModelPtr == nullptr){
		return nullptr;
	}

	imtbase::CTreeItemModel* filterSearchModelPtr = headersDataModelPtr->GetTreeItemModel("filterSearch");
	if (filterSearchModelPtr == nullptr){
		return nullptr;
	}
	
	sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0 complexFilter;
	
	sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0 groupFilter;
	groupFilter.logicalOperation = sdl::imtbase::ComplexCollectionFilter::LogicalOperation::Or;
	
	QList<sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0> fieldList;
	for (int i = 0; i < filterSearchModelPtr->GetItemsCount(); i++){
		sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0 fieldFilter;
		fieldFilter.fieldId = filterSearchModelPtr->GetData("id", i).toByteArray();
		fieldFilter.filterValue = text;
		fieldFilter.filterValueType = sdl::imtbase::ComplexCollectionFilter::ValueType::String;
		
		QList<sdl::imtbase::ComplexCollectionFilter::FilterOperation> filterOperations;
		filterOperations << sdl::imtbase::ComplexCollectionFilter::FilterOperation::Contains;
		fieldFilter.filterOperations = filterOperations;
		
		fieldList << fieldFilter;
	}
	
	groupFilter.fieldFilters = fieldList;
	complexFilter.fieldsFilter = groupFilter;

	imtgql::CGqlObject input;
	imtgql::CGqlObject viewParams;
	viewParams.InsertField("offset", 0);
	viewParams.InsertField("count", -1);
	
	imtgql::CGqlObject complexFilterGqlOblect;
	if (complexFilter.WriteToGraphQlObject(complexFilterGqlOblect)){
		viewParams.InsertField("filterModel", complexFilterGqlOblect);
	}
	
	input.InsertField("viewParams", viewParams);
	gqlRequest.AddParam("input", input);

	imtgql::CGqlObject items;
	items.InsertField("id");
	items.InsertField("name");
	items.InsertField("typeId");
	items.InsertField("description");
	gqlRequest.AddField("items", items);
	
	istd::TDelPtr<imtbase::CTreeItemModel> resultModelPtr = BaseClass::ListObjects(gqlRequest, errorMessage);
	if (!resultModelPtr.IsValid()){
		return nullptr;
	}

	imtbase::CTreeItemModel* dataModelPtr = resultModelPtr->GetTreeItemModel("data");
	if (dataModelPtr == nullptr){
		return nullptr;
	}

	imtbase::CTreeItemModel* itemsModelPtr = dataModelPtr->GetTreeItemModel("items");
	if (itemsModelPtr == nullptr){
		return nullptr;
	}

	imtbase::CSearchResults* searchResultsPtr = new imtbase::CSearchResults();
	for (int i = 0; i < itemsModelPtr->GetItemsCount(); i++){
		imtbase::ISearchResults::SearchResult searchResult;

		searchResult.contextId = itemsModelPtr->GetData("id", i).toByteArray();
		searchResult.resultName = itemsModelPtr->GetData("name", i).toString();
		searchResult.resultDescription = itemsModelPtr->GetData("description", i).toString();
		searchResult.contextTypeId = itemsModelPtr->GetData("typeId", i).toByteArray();

		searchResultsPtr->AddSearchResult(searchResult);
	}

	return searchResultsPtr;
}


} // namespace imtservergql


