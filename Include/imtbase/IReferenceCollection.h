// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>
#include <imtbase/ICollectionObjectLink.h>


namespace imtbase
{


/**
	Interface for collection of references to the objects that can be identified (e.g. in some abstract storage) using their IDs.
	\ingroup Collection
*/
class IReferenceCollection:
			virtual public IObjectCollectionInfo,
			virtual public iser::ISerializable
{
public:
	typedef std::shared_ptr<const ICollectionObjectLink> ICollectionObjectLinkPtr;

	virtual ICollectionObjectLinkPtr GetObjectLink(const Id& objectId, const QByteArray& repositoryId = QByteArray()) const = 0;

	/**
		Insert a new reference into the collection.
		\param objectId			unique ID of the object in the related repository to be referenced
		\param repositoryId		Repository identifier
	*/
	virtual bool InsertReference(const Id& objectId, const QByteArray& repositoryId = QByteArray()) = 0;

	/**
		Remove a given reference from the collection.
	*/
	virtual bool RemoveReference(const Id& objectId) = 0;

	/**
		Remove all references from the collection.
	*/
	virtual bool RemoveAllReferences() = 0;
};


typedef istd::TUniqueInterfacePtr<IReferenceCollection> IReferenceCollectionUniquePtr;
typedef istd::TSharedInterfacePtr<IReferenceCollection> IReferenceCollectionSharedPtr;


} // namespace imtbase


