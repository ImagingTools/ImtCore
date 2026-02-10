// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/TDelPtr.h>
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
	inline bool GetLinkedObject(istd::TSharedInterfacePtr<Object>& dataObject) const
	{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (GetLinkedObject(dataPtr)){
			dataObject.MoveCastedPtr(dataPtr->CloneMe());

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


