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
	virtual void SetObject(const QByteArray& objectId, istd::IChangeable* objectPtr);

	/**
		Register an existing object in the collection.
		\note Object memory is not managed by the collection.
	*/
	virtual QByteArray RegisterObject(
				const QByteArray& objectId,
				const QByteArray& typeId,
				const QString& name,
				const QString& description = QString(),
				istd::IChangeable* objectPtr = nullptr);

	// reimplemented (IObjectCollection)
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const QByteArray& proposedObjectId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

protected:
	// reimplemented (CObjectCollectionBase)
	virtual istd::IChangeable* CreateObjectInstance(const QByteArray& typeId) const;
	virtual void DestroyObjectInstance(istd::IChangeable* objectPtr) const override;
};


} // namespace imtbase


