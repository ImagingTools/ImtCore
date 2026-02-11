// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual Id InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const Id& proposedElementId = Id(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
	virtual bool SetObjectData(
				const Id& objectId,
				const istd::IChangeable& object,
				CompatibilityMode mode = CM_WITHOUT_REFS,
				const IOperationContext* operationContextPtr = nullptr) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

protected:
	// reimplemented (CObjectCollectionBase)
	virtual istd::IChangeableUniquePtr CreateObjectInstance(const QByteArray& typeId) const override;
	virtual IObjectCollection* CreateSubCollectionInstance() const override;
	virtual bool InsertObjectIntoCollection(ObjectInfo info) override;

private:
	int m_operationFlags;
	IObjectCollection& m_parent;
	bool m_isObjectProxy;
};


} // namespace imtbase


