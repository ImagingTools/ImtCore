#pragma once


// ACF includes
#include <iprm/TParamsPtr.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/ICollectionFilter.h>



namespace imtbase
{


template <class Base>
class TFilterableCollectionWrap: public Base
{
public:
	typedef Base BaseClass;

	// pseudo-reimplemented (ICollectionInfo)
	virtual int GetElementsCount(const iprm::IParamsSet* selectionParamPtr = nullptr) const override;
	virtual imtbase::ICollectionInfo::Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
protected:
	virtual imtbase::ICollectionInfo::Ids GetFilteredElementIds(const iprm::IParamsSet& filterParams) const;
	virtual bool IsAcceptedByFilter(const QByteArray& objectId, const iprm::IParamsSet& filterParams) const;
};


// reimplemented (ICollectionInfo)

template <class Base>
int TFilterableCollectionWrap<Base>::GetElementsCount(const iprm::IParamsSet* selectionParamPtr) const
{
	if (selectionParamPtr != nullptr){
		return GetFilteredElementIds(*selectionParamPtr).count();
	}

	return BaseClass::GetElementsCount(selectionParamPtr);
}


template <class Base>
imtbase::ICollectionInfo::Ids TFilterableCollectionWrap<Base>::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamPtr) const
{
	ICollectionInfo::Ids retVal;

	Q_ASSERT(offset >= 0);

	if (selectionParamPtr != nullptr){
		Ids filteredIds = GetFilteredElementIds(*selectionParamPtr);
		int objectsCount = count >= 0 ? qMin(count, filteredIds.count()) : filteredIds.count();

		for (int i = offset; i < objectsCount; i++){
			retVal.push_back(filteredIds[i]);
		}
	}
	else {
		retVal = BaseClass::GetElementIds(offset, count, selectionParamPtr);
	}

	return retVal;
}



// protected methods

template <class Base>
imtbase::ICollectionInfo::Ids TFilterableCollectionWrap<Base>::GetFilteredElementIds(const iprm::IParamsSet& filterParams) const
{
	Ids retVal;

	Ids elementIds = GetElementIds();

	for (const QByteArray& id : elementIds){
		if (IsAcceptedByFilter(id, filterParams)){
			retVal.push_back(id);
		}
	}

	return retVal;
}


template <class Base>
bool TFilterableCollectionWrap<Base>::IsAcceptedByFilter(const QByteArray& objectId, const iprm::IParamsSet& filterParams) const
{
	iprm::TParamsPtr<imtbase::ICollectionFilter> filterParamPtr(&filterParams, "Filter");
	if (filterParamPtr.IsValid()){
		QString textFilter = filterParamPtr->GetTextFilter();
		if (textFilter.isEmpty()){
			return true;
		}

		QByteArrayList relatedIds = filterParamPtr->GetFilteringInfoIds();
		if (relatedIds.isEmpty()){
			return true;
		}

		imtbase::IMetaInfoCreator::MetaInfoPtr metaInfoPtr;
		if (GetDataMetaInfo(objectId, metaInfoPtr)){
			idoc::IDocumentMetaInfo::MetaInfoTypes metaInfoTypes = metaInfoPtr->GetMetaInfoTypes();

			for (int type : metaInfoTypes){
				QString metaInfoName = metaInfoPtr->GetMetaInfoName(type);
				if (relatedIds.contains(metaInfoName.toUtf8())){
					QString metaInfoValue = metaInfoPtr->GetMetaInfo(type).toString();
					if (metaInfoValue.contains(textFilter, Qt::CaseInsensitive)){
						return true;
					}
				}
			}
		}

		idoc::CStandardDocumentMetaInfo collectionItemMetaInfo;
		if (GetCollectionItemMetaInfo(objectId, collectionItemMetaInfo)){
			idoc::IDocumentMetaInfo::MetaInfoTypes metaInfoTypes = collectionItemMetaInfo.GetMetaInfoTypes();

			for (int type : metaInfoTypes){
				QString metaInfoName = collectionItemMetaInfo.GetMetaInfoName(type);
				if (relatedIds.contains(metaInfoName.toUtf8())){
					QString metaInfoValue = collectionItemMetaInfo.GetMetaInfo(type).toString();
					if (metaInfoValue.contains(textFilter, Qt::CaseInsensitive)){
						return true;
					}
				}
			}
		}

		if (relatedIds.contains("Name")){
			QString metaInfoValue = GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
			if (metaInfoValue.contains(textFilter, Qt::CaseInsensitive)){
				return true;
			}
		}

		return false;
	}

	return true;
}

} // namespace imtbase


