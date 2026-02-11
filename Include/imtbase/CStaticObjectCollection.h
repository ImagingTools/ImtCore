// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>


namespace imtbase
{


/**
	Implementation of a static object collection.
	\ingroup Collection
*/
class CStaticObjectCollection: public CObjectCollectionBase
{
public:
	typedef CObjectCollectionBase BaseClass;

	/**
		Replace an existing collection object with another one.
	*/
	virtual void SetObject(const QByteArray& objectId, DataPtr objectPtr);

	/**
		Register an existing object in the collection.
		\note Object memory is not managed by the collection.
	*/
	virtual QByteArray RegisterObject(
				const QByteArray& objectId,
				const QByteArray& typeId,
				const QString& name,
				const QString& description = QString(),
				DataPtr objectPtr = DataPtr());

	// reimplemented (IObjectCollection)
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const QByteArray& proposedObjectId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

protected:
	// reimplemented (CObjectCollectionBase)
	virtual istd::IChangeableUniquePtr CreateObjectInstance(const QByteArray& typeId) const override;
	virtual IObjectCollection* CreateSubCollectionInstance() const override;
};


} // namespace imtbase


