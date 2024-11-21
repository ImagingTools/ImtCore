#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtbase/CCollectionFilter.h>
#include <imtbase/CSearchResults.h>
#include <imtbase/ISearchController.h>
#include <imtbase/IObjectCollectionIterator.h>


namespace imtbase
{


template <class ObjectCollection>
class TObjectCollectionSearchControllerCompWrap:
			public ObjectCollection,
			virtual public ISearchController
{
public:
	typedef ObjectCollection BaseClass;

	I_BEGIN_COMPONENT(TObjectCollectionSearchControllerCompWrap);
		I_REGISTER_INTERFACE(imtbase::ISearchController);
		I_ASSIGN_MULTI_0(m_headersIdsAttrPtr, "SearchableHeadersIds", "Fields of the collection for which the global search is looking for data", true);
	I_END_COMPONENT;

	// reimplemented (ISearchController)
	virtual QByteArray GetControllerId() const override;
	virtual const imtbase::ISearchResults* Search(const QString& text) const override;

protected:
	I_MULTIATTR(QByteArray, m_headersIdsAttrPtr);
};


// public methods

template<class ObjectCollection>
QByteArray TObjectCollectionSearchControllerCompWrap<ObjectCollection>::GetControllerId() const
{
	return QByteArray();
}


template<class ObjectCollection>
const imtbase::ISearchResults* TObjectCollectionSearchControllerCompWrap<ObjectCollection>::Search(const QString& text) const
{
	imtbase::CCollectionFilter collectionFilter;
	collectionFilter.SetTextFilter(text);

	QByteArrayList headersIds;

	for (int i = 0; i < m_headersIdsAttrPtr.GetCount(); i++){
		headersIds << m_headersIdsAttrPtr[i];
	}

	collectionFilter.SetFilteringInfoIds(headersIds);

	iprm::CParamsSet paramsSet;
	paramsSet.SetEditableParameter("Filter", &collectionFilter);

	imtbase::CSearchResults* searchResultsPtr = new imtbase::CSearchResults();

	istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(BaseClass::CreateObjectCollectionIterator(QByteArray(), 0, -1, &paramsSet));
	if (objectCollectionIterator != nullptr){
		while (objectCollectionIterator->Next()){
			QByteArray objectId = objectCollectionIterator->GetObjectId();
			idoc::MetaInfoPtr metaInfoPtr = objectCollectionIterator->GetDataMetaInfo();
			if (metaInfoPtr.IsValid()){
				imtbase::ISearchResults::SearchResult searchResult;

				QString name = metaInfoPtr->GetMetaInfo(imtbase::ICollectionInfo::EIT_NAME).toString();
				QString description = metaInfoPtr->GetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

				searchResult.contextId = objectId;
				searchResult.contextTypeId = BaseClass::GetObjectTypeId(objectId);
				searchResult.resultName = name;
				searchResult.resultDescription = description;

				searchResultsPtr->AddSearchResult(searchResult);
			}
		}
	}

	return searchResultsPtr;
}


} // namespace imtbase


