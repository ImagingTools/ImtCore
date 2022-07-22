#include <imtbase/CObjectMetaInfoUniquenessValidatorComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtbase
{


// public methods

// reimplemented (IMetaInfoUniquenessChecker)

int CObjectMetaInfoUniquenessValidatorComp::IsUniqueValue(const imtbase::IObjectCollection* collectionPtr, const QByteArray& objectId, const QVariant& value) const
{
	int retVal = RF_IS_UNIQUE;
	bool isUnique = true;

	int collectionCount = GetCollectionCount();

	for (int collectionIndex = 0; collectionIndex < collectionCount && isUnique; collectionIndex++){
		imtbase::IObjectCollection* currentCollectionPtr = m_collectionCompPtr[collectionIndex];
		int metaInfoType = m_metaInfoTypeAttrPtr[collectionIndex];

		if (currentCollectionPtr == nullptr){
			retVal |= RF_INVALID_COLLECTION_REFERENCE;
			continue;
		}

		imtbase::ICollectionInfo::Ids objectIds = currentCollectionPtr->GetElementIds();
		int objectCount = objectIds.count();
		for (int objectIndex = 0; objectIndex < objectCount && isUnique; objectIndex++){
			QByteArray currentObjectId = objectIds[objectIndex];

			idoc::MetaInfoPtr metaInfoPtr;
			QSet<int> metaInfoTypes;
			if (metaInfoType == -1){
				metaInfoTypes.insert(-1);

				idoc::CStandardDocumentMetaInfo* standardDocumentMetaInfoPtr = new idoc::CStandardDocumentMetaInfo();

				standardDocumentMetaInfoPtr->SetMetaInfo(
							-1, currentCollectionPtr->GetElementInfo(currentObjectId, imtbase::ICollectionInfo::EIT_NAME).toString());

				metaInfoPtr.SetPtr(standardDocumentMetaInfoPtr);
			}
			else{
				metaInfoTypes = metaInfoPtr->GetMetaInfoTypes();

				metaInfoPtr = currentCollectionPtr->GetDataMetaInfo(currentObjectId);
				if (!metaInfoPtr.IsValid()){
					retVal |= RF_MISSING_METAINFO;
					continue;
				}
			}

			if (!metaInfoTypes.contains(metaInfoType)){
				retVal |= RF_MISSING_METAINFO_TYPE;
				continue;
			}

			QVariant metaInfoValue = metaInfoPtr->GetMetaInfo(metaInfoType);
			if (metaInfoValue.type() != value.type()){
				retVal |= RF_INCOMPATIBLE_VALUE_TYPE;
				continue;
			}

			if (currentCollectionPtr != collectionPtr || currentObjectId != objectId){
				if (metaInfoValue == value){
					retVal |= RF_NOT_UNIQUE;
					isUnique = false;
				}
			}
		}
	}

	return retVal;
}


// private methods

int CObjectMetaInfoUniquenessValidatorComp::GetCollectionCount() const
{
	int count = m_collectionCompPtr.GetCount();
	count = qMin(count, m_metaInfoTypeAttrPtr.GetCount());

	return count;
}


} // namespace imtbase


