#include <imtbase/CCollectionObjectExtractorComp.h>


namespace imtbase
{


// public methods

// reimplemented (IObjectExtractor)

QByteArrayList CCollectionObjectExtractorComp::GetSupportedObjectIds(const istd::IChangeable* compositePtr) const
{
	const imtbase::IObjectCollection* collectionPtr = dynamic_cast<const imtbase::IObjectCollection*>(compositePtr);
	if (collectionPtr != nullptr){
		return collectionPtr->GetElementIds().toList();
	}
}


const istd::IChangeable* CCollectionObjectExtractorComp::ExtractObject(const istd::IChangeable* compositePtr, const QByteArray& objectId) const
{
	const imtbase::IObjectCollection* collectionPtr = dynamic_cast<const imtbase::IObjectCollection*>(compositePtr);
	if (collectionPtr != nullptr){
		return collectionPtr->GetObjectPtr(objectId);
	}

	return nullptr;
}


} // namespace imtbase


