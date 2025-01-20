#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtbase/IIdentifiable.h>
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


class ICollectionObjectLink: virtual public IIdentifiable, virtual public iser::IObject
{
public:
	/**
		Get collection containing the referenced/linked object.
	*/
	virtual const imtbase::IObjectCollection* GetCollection() const = 0;

	template <class Object>
	inline bool GetLinkedObject(istd::TSmartPtr<Object>& dataObject) const
	{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (GetLinkedObject(dataPtr)){
			dataObject.SetCastedOrRemove(dataPtr->CloneMe());

			return dataObject.IsValid();
		}

		return false;
	}

	inline bool GetLinkedObject(imtbase::IObjectCollection::DataPtr& dataObject) const
	{
		const imtbase::IObjectCollection* collectionPtr = GetCollection();
		if (collectionPtr != nullptr){
			QByteArray objectId = GetObjectUuid();
			if (!objectId.isEmpty()){
				return collectionPtr->GetObjectData(objectId, dataObject);
			}
		}

		return false;
	}
};


} // namespace imtbase


