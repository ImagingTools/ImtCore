// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/TSingleFactory.h>
#include <istd/CChangeNotifier.h>
#include <imod/TModelWrap.h>
#include <imod/CModelUpdateBridge.h>
#include <iprm/COptionsManager.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>

// ImtCore includes
#include <imtbase/CObjectCollection.h>


namespace imtbase
{


/**
	Implementation of an object collection using aggregation.
*/
template <class BaseInterface, class ObjectImpl>
class TAggergatedObjectCollectionWrap:
			virtual public BaseInterface,
			virtual public IObjectCollection,
			virtual public iser::ISerializable
{
public:
	TAggergatedObjectCollectionWrap(
				const QByteArray& typeId,
				const QString& typeName,
				const QByteArray& collectionTagName);
	virtual ~TAggergatedObjectCollectionWrap();

	void SetOperationFlags(int flags, const QByteArray& objectId = QByteArray());

	// reimplemented (IObjectCollection)
	virtual const IRevisionController* GetRevisionController() const override;
	virtual const ICollectionDataController* GetDataController() const override;
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
	virtual bool RemoveElements(const Ids& elementIds, const IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveElementSet(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RestoreObjects(
				const Ids& objectIds,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RestoreObjectSet(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;
	virtual bool GetObjectData( const QByteArray& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
	virtual bool SetObjectData( const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS, const IOperationContext* operationContextPtr = nullptr) override;
	virtual IObjectCollectionUniquePtr CreateSubCollection(int offset, int count, const iprm::IParamsSet *selectionParamsPtr) const override;
	virtual IObjectCollectionIterator* CreateObjectCollectionIterator(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual int GetElementsCount(const iprm::IParamsSet* selectionParamPtr = nullptr, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool GetSubsetInfo(
				ICollectionInfo& subsetInfo,
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool SetElementName(const QByteArray& objectId, const QString& objectName, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementDescription(const QByteArray& objectId, const QString& objectDescription, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementEnabled(const QByteArray& objectId, bool isEnabled = true, ilog::IMessageConsumer* logPtr = nullptr) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	imod::TModelWrap<CObjectCollection> m_collection;

	imod::CModelUpdateBridge m_updateBridge;

	iprm::COptionsManager m_types;

	QByteArray m_collectionTagName;

};


// public methods

template<class BaseInterface, class ObjectImpl>
inline TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::TAggergatedObjectCollectionWrap(
			const QByteArray& typeId,
			const QString& typeName,
			const QByteArray& collectionTagName)
	:m_updateBridge(this, imod::CModelUpdateBridge::UF_SOURCE),
	m_collectionTagName(collectionTagName)
{
	m_collection.AttachObserver(&m_updateBridge);

	typedef istd::TSingleFactory<istd::IChangeable, imod::TModelWrap<ObjectImpl>> FactoryImpl;

	m_collection.RegisterFactory<FactoryImpl>(typeId);

	m_types.InsertOption(typeName, typeId);
}


template<class BaseInterface, class ObjectImpl>
inline TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::~TAggergatedObjectCollectionWrap()
{
	m_updateBridge.EnsureModelsDetached();
}


template<class BaseInterface, class ObjectImpl>
void TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::SetOperationFlags(int flags, const QByteArray& objectId)
{
	m_collection.SetOperationFlags(flags, objectId);
}


// reimplemented (IObjectCollection)

template<class BaseInterface, class ObjectImpl>
inline const IRevisionController* TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetRevisionController() const
{
	return m_collection.GetRevisionController();
}


template<class BaseInterface, class ObjectImpl>
inline const ICollectionDataController* TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetDataController() const
{
	return m_collection.GetDataController();
}


template<class BaseInterface, class ObjectImpl>
inline int TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetOperationFlags(const QByteArray& objectId) const
{
	return m_collection.GetOperationFlags(objectId);
}


template<class BaseInterface, class ObjectImpl>
inline QByteArray TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const IOperationContext* operationContextPtr)
{
	return m_collection.InsertNewObject(
				typeId,
				name,
				description,
				defaultValuePtr,
				proposedObjectId,
				dataMetaInfoPtr,
				collectionItemMetaInfoPtr,
				operationContextPtr);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::RemoveElements(
			const Ids& elementIds,
			const IOperationContext* operationContextPtr)
{
	return m_collection.RemoveElements(elementIds, operationContextPtr);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::RemoveElementSet(
			const iprm::IParamsSet* selectionParamsPtr,
			const IOperationContext* operationContextPtr)
{
	return m_collection.RemoveElementSet(selectionParamsPtr, operationContextPtr);
}


template<class BaseInterface, class ObjectImpl>
bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::RestoreObjects(
			const Ids& objectIds,
			const IOperationContext* operationContextPtr)
{
	return m_collection.RestoreObjects(objectIds, operationContextPtr);
}


template<class BaseInterface, class ObjectImpl>
bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::RestoreObjectSet(
			const iprm::IParamsSet* selectionParamsPtr,
			const IOperationContext* operationContextPtr)
{
	return m_collection.RestoreObjectSet(selectionParamsPtr, operationContextPtr);
}


template<class BaseInterface, class ObjectImpl>
inline const istd::IChangeable* TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetObjectPtr(const QByteArray& objectId) const
{
	return m_collection.GetObjectPtr(objectId);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr, const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	return m_collection.GetObjectData(objectId, dataPtr);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::SetObjectData(
			const QByteArray& objectId,
			const istd::IChangeable& object,
			CompatibilityMode mode,
			const IOperationContext* operationContextPtr)
{
	return m_collection.SetObjectData(objectId, object, mode, operationContextPtr);
}


template<class BaseInterface, class ObjectImpl>
inline IObjectCollectionUniquePtr TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	return m_collection.CreateSubCollection(offset, count, selectionParamsPtr);
}


template<class BaseInterface, class ObjectImpl>
IObjectCollectionIterator* TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::CreateObjectCollectionIterator(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	return m_collection.CreateObjectCollectionIterator(objectId, offset, count, selectionParamsPtr);
}


// reimplemented (IObjectCollectionInfo)

template<class BaseInterface, class ObjectImpl>
inline const iprm::IOptionsList* TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetObjectTypesInfo() const
{
	return &m_types;
}


template<class BaseInterface, class ObjectImpl>
inline ICollectionInfo::Id TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	return m_types.GetOptionId(0);
}

template<class BaseInterface, class ObjectImpl>
inline idoc::MetaInfoPtr TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetDataMetaInfo(const Id& objectId) const
{
	return m_collection.GetDataMetaInfo(objectId);
}


// reimplemented (ICollectionInfo)

template<class BaseInterface, class ObjectImpl>
inline int TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetElementsCount(const iprm::IParamsSet* selectionParamPtr, ilog::IMessageConsumer* logPtr) const
{
	return m_collection.GetElementsCount(selectionParamPtr, logPtr);
}


template<class BaseInterface, class ObjectImpl>
inline ICollectionInfo::Ids TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	return m_collection.GetElementIds(offset, count, selectionParamsPtr, logPtr);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetSubsetInfo(
			ICollectionInfo& subsetInfo,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	return m_collection.GetSubsetInfo(subsetInfo, offset, count, selectionParamsPtr, logPtr);
}


template<class BaseInterface, class ObjectImpl>
inline QVariant TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* logPtr) const
{
	return m_collection.GetElementInfo(elementId, infoType, logPtr);
}


template<class BaseInterface, class ObjectImpl>
inline idoc::MetaInfoPtr TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr) const
{
	return m_collection.GetElementMetaInfo(elementId, logPtr);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::SetElementName(const QByteArray& objectId, const QString& objectName, ilog::IMessageConsumer* logPtr)
{
	return m_collection.SetElementName(objectId, objectName, logPtr);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::SetElementDescription(const QByteArray& objectId, const QString& objectDescription, ilog::IMessageConsumer* logPtr)
{
	return m_collection.SetElementDescription(objectId, objectDescription, logPtr);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::SetElementEnabled(const QByteArray& objectId, bool isEnabled, ilog::IMessageConsumer* logPtr)
{
	return m_collection.SetElementEnabled(objectId, isEnabled, logPtr);
}


// reimplemented (iser::ISerializable)

template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag collectionTag(m_collectionTagName, "", iser::CArchiveTag::TT_GROUP);
	bool retVal = archive.BeginTag(collectionTag);
	retVal = retVal && m_collection.Serialize(archive);
	retVal = retVal && archive.EndTag(collectionTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>* sourcePtr = dynamic_cast<const TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		return m_collection.CopyFrom(sourcePtr->m_collection, mode);
	}

	return false;
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	return m_collection.ResetData();
}


} // namespace imtbase


