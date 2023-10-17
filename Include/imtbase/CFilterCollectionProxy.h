#pragma once


// ACF includes
#include <istd/TComposedFactory.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>


namespace imtbase
{


/**
	Implementation of a general data object collection.
	\ingroup Collection
*/
class CFilterCollectionProxy:
			public CObjectCollectionBase,
			public istd::TComposedFactory<istd::IChangeable>
{
public:
	typedef CObjectCollectionBase BaseClass;
	typedef istd::TComposedFactory<istd::IChangeable> BaseClass2;

	CFilterCollectionProxy(IObjectCollection& parent, bool isObjectProxy = false);

	void SetOperationFlags(int flags, const QByteArray& objectId = QByteArray());

	// reimplemented (imtbase::IObjectCollection)
	int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual Id InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				DataPtr defaultValuePtr = DataPtr(),
				const Id& proposedElementId = Id(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr) const override;
	virtual bool SetObjectData(
				const Id& objectId,
				const istd::IChangeable& object,
				CompatibilityMode mode = CM_WITHOUT_REFS,
				const IOperationContext* operationContextPtr = nullptr) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

protected:
	// reimplemented (CObjectCollectionBase)
	virtual DataPtr CreateObjectInstance(const QByteArray& typeId) const override;
	virtual bool InsertObjectIntoCollection(ObjectInfo info) override;

private:
	int m_operationFlags;
	IObjectCollection& m_parent;
	bool m_isObjectProxy;
};


} // namespace imtbase


