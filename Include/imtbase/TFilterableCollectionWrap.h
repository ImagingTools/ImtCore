#pragma once


// ACF includes
#include <iprm/TParamsPtr.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/ICollectionFilter.h>
#include <imtbase/CObjectCollectionMetaInfo.h>


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
	virtual imtbase::ICollectionInfo::Ids GetSortedElementIds(
				const imtbase::ICollectionInfo::Ids filteredIds,
				const iprm::IParamsSet& filterParams) const;
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
		imtbase::ICollectionInfo::Ids filteredIds = GetFilteredElementIds(*selectionParamPtr);
		imtbase::ICollectionInfo::Ids sortedIds = GetSortedElementIds(filteredIds, *selectionParamPtr);
		int objectsCount = count >= 0 ? qMin(count, sortedIds.count()) : sortedIds.count();
		if ((sortedIds.count() - offset - objectsCount) < 0){
			for (int i = offset; i < sortedIds.count(); i++){
				retVal.push_back(sortedIds[i]);
			}
		}
		else{
			int countIds = offset + objectsCount;
			for (int i = offset; i < countIds; i++){
			retVal.push_back(sortedIds[i]);
		}
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
	imtbase::ICollectionInfo::Ids retVal;

	imtbase::ICollectionInfo::Ids elementIds = GetElementIds();

	for (const QByteArray& id : elementIds){
		if (IsAcceptedByFilter(id, filterParams)){
			retVal.push_back(id);
		}
	}

	return retVal;
}


template <class Base>
imtbase::ICollectionInfo::Ids TFilterableCollectionWrap<Base>::GetSortedElementIds(
			const imtbase::ICollectionInfo::Ids filteredIds,
			const iprm::IParamsSet& filterParams) const
{
	imtbase::ICollectionInfo::Ids retVal;
	iprm::TParamsPtr<imtbase::ICollectionFilter> filterParamPtr(&filterParams, "Filter");
	if (filterParamPtr.IsValid()){
		imtbase::ICollectionFilter::SortingOrder sortingOrder = filterParamPtr->GetSortingOrder();
		if (sortingOrder == imtbase::ICollectionFilter::SO_NO_ORDER){
			return filteredIds;
		}
		else{
			QByteArrayList relatedIds = filterParamPtr->GetSortingInfoIds();
			if (!relatedIds.isEmpty()){
				QList<QPair<QString, QByteArray>> listObjects;
				for (int i = 0; i < filteredIds.count(); i++){
					QByteArray objectId = filteredIds[i];

					idoc::MetaInfoPtr metaInfoPtr;
					if (relatedIds.contains("Name")){
						QString metaInfoValue = BaseClass::GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
						QPair<QString, QByteArray> objectPair = {metaInfoValue, objectId};
						listObjects.append(objectPair);
					}
					else{
						metaInfoPtr = BaseClass::GetDataMetaInfo(objectId);
						if (metaInfoPtr.IsValid()){
							idoc::IDocumentMetaInfo::MetaInfoTypes metaInfoTypes = metaInfoPtr->GetMetaInfoTypes();
							for (int type : metaInfoTypes){
								QByteArray metaInfoId = metaInfoPtr->GetMetaInfoId(type);
								if (relatedIds[0] == metaInfoId){
									QString objectName = metaInfoPtr->GetMetaInfo(type).toString();
									QPair<QString, QByteArray> objectPair = {objectName, objectId};
									listObjects.append(objectPair);
									break;
								}
							}
						}

						idoc::MetaInfoPtr collectionItemMetaInfoPtr = BaseClass::GetElementMetaInfo(objectId);
						if (collectionItemMetaInfoPtr.IsValid()){
							idoc::IDocumentMetaInfo::MetaInfoTypes metaInfoTypes = collectionItemMetaInfoPtr->GetMetaInfoTypes();

							for (int type : metaInfoTypes){
								QByteArray metaInfoId = collectionItemMetaInfoPtr->GetMetaInfoId(type);
								if (relatedIds[0] == metaInfoId){
									QString objectName = collectionItemMetaInfoPtr->GetMetaInfo(type).toString();
									QPair<QString, QByteArray> objectPair = { objectName, objectId };
									listObjects.append(objectPair);
								}
							}
						}
					}
				}
				if (!listObjects.isEmpty()){
					if (sortingOrder == imtbase::ICollectionFilter::SO_ASC){
						std::sort(listObjects.begin(), listObjects.end(), [](const QPair<QString, QByteArray> &a, const QPair<QString, QByteArray> &b){
							if(a.first.isEmpty())
								return true;
							if (b.first.isEmpty())
								return false;
							if (a.first.toUpper() == b.first.toUpper())
								return a.first > b.first;
							return a.first.toUpper() < b.first.toUpper();
						});
					}
					else{
						std::sort(listObjects.rbegin(), listObjects.rend(), [](const QPair<QString, QByteArray> &a, const QPair<QString, QByteArray> &b){
							if(a.first.isEmpty())
								return true;
							if (b.first.isEmpty())
								return false;
							if (a.first.toUpper() == b.first.toUpper())
								return a.first > b.first;
							return a.first.toUpper() < b.first.toUpper();
						});
					}
					for(int index = 0; index < listObjects.count(); index++){
						retVal.append(listObjects[index].second);
					}
					return retVal;
				}
			}
			else{
				return filteredIds;
			}
		}
	}
	else{
		return filteredIds;
	}
	return filteredIds;
}


template <class Base>
bool TFilterableCollectionWrap<Base>::IsAcceptedByFilter(const QByteArray& objectId, const iprm::IParamsSet& filterParams) const
{
	iprm::TParamsPtr<imtbase::ICollectionFilter> filterParamPtr(&filterParams, "Filter");
	if (filterParamPtr.IsValid()){
		QByteArray allowedTypeId = filterParamPtr->GetObjectTypeId();
		if (!allowedTypeId.isEmpty()){
			QByteArray typeId = BaseClass::GetObjectTypeId(objectId);
			if (typeId != allowedTypeId){
				return false;
			}
		}

		QString textFilter = filterParamPtr->GetTextFilter();
		if (textFilter.isEmpty()){
			return true;
		}

		QByteArrayList relatedIds = filterParamPtr->GetFilteringInfoIds();
		if (relatedIds.isEmpty()){
			return true;
		}

		idoc::MetaInfoPtr dataMetaInfoPtr = BaseClass::GetDataMetaInfo(objectId);
		if (dataMetaInfoPtr.IsValid()){
			idoc::IDocumentMetaInfo::MetaInfoTypes metaInfoTypes = dataMetaInfoPtr->GetMetaInfoTypes();

			for (int type : metaInfoTypes){
				QByteArray metaInfoId = dataMetaInfoPtr->GetMetaInfoId(type);
				if (relatedIds.contains(metaInfoId)){
					QString metaInfoValue = dataMetaInfoPtr->GetMetaInfo(type).toString();
					if (metaInfoValue.contains(textFilter, Qt::CaseInsensitive)){
						return true;
					}
				}
			}
		}

		idoc::MetaInfoPtr elementMetaInfoPtr = BaseClass::GetElementMetaInfo(objectId);
		if (elementMetaInfoPtr.IsValid()){
			idoc::IDocumentMetaInfo::MetaInfoTypes metaInfoTypes = elementMetaInfoPtr->GetMetaInfoTypes();

			for (int type : metaInfoTypes){
				QByteArray metaInfoId = elementMetaInfoPtr->GetMetaInfoId(type);
				if (relatedIds.contains(metaInfoId)){
					QString metaInfoValue = elementMetaInfoPtr->GetMetaInfo(type).toString();
					if (metaInfoValue.contains(textFilter, Qt::CaseInsensitive)){
						return true;
					}
				}
			}
		}

		if (relatedIds.contains("Name")){
			QString metaInfoValue = BaseClass::GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
			if (metaInfoValue.contains(textFilter, Qt::CaseInsensitive)){
				return true;
			}
		}

		return false;
	}

	return true;
}


} // namespace imtbase


