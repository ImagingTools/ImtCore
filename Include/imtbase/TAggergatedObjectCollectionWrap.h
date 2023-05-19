#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QDateTime>

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
			virtual public imtbase::IObjectCollection,
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
	virtual const imtbase::IRevisionController* GetRevisionController() const override;
	virtual const imtbase::ICollectionDataController* GetDataController() const override;
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual Id InsertNewBranch(
				const Id& parentId,
				const QString& name,
				const QString& description,
				const Id& proposedElementId = Id(),
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr) override;
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				DataPtr defaultValuePtr = DataPtr(),
				const QByteArray& proposedObjectId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				const Id& parentId = Id()) override;
	virtual bool RemoveElement(const QByteArray& elementId) override;
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;
	virtual bool GetObjectData( const QByteArray& objectId, DataPtr& dataPtr) const override;
	virtual bool SetObjectData( const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual imtbase::IObjectCollection* CreateSubCollection(int offset, int count, const iprm::IParamsSet *selectionParamsPtr, const Id &parentId, int iterationFlags) const override;
	virtual imtbase::IObjectCollectionIterator* CreateObjectCollectionIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const Id& parentId = Id(),
				int iterationFlags = IF_RECURSIVE | IF_LEAF_ONLY) const override;
	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual int GetElementsCount(
				const iprm::IParamsSet* selectionParamPtr = nullptr,
				const Id& parentId = Id(),
				int iterationFlags = IF_RECURSIVE | IF_LEAF_ONLY) const override;
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const Id& parentId = Id(),
				int iterationFlags = IF_RECURSIVE | IF_LEAF_ONLY) const override;
	virtual bool GetSubsetInfo(
				imtbase::ICollectionInfo& subsetInfo,
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const Id& parentId = Id(),
				int iterationFlags = IF_RECURSIVE | IF_LEAF_ONLY) const override;
	virtual Id GetParentId(const Id& elementId) const override;
	virtual Ids GetElementPath(const Id& elementId) const override;
	virtual bool IsBranch(const Id& elementId) const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId) const override;
	virtual bool SetElementName(const QByteArray& objectId, const QString& objectName) override;
	virtual bool SetElementDescription(const QByteArray& objectId, const QString& objectDescription) override;
	virtual bool SetElementEnabled(const QByteArray& objectId, bool isEnabled = true) override;

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
inline const imtbase::IRevisionController* TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetRevisionController() const
{
	return m_collection.GetRevisionController();
}


template<class BaseInterface, class ObjectImpl>
inline const imtbase::ICollectionDataController* TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetDataController() const
{
	return m_collection.GetDataController();
}


template<class BaseInterface, class ObjectImpl>
inline int TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetOperationFlags(const QByteArray& objectId) const
{
	return m_collection.GetOperationFlags(objectId);
}


template<class BaseInterface, class ObjectImpl>
inline ICollectionInfo::Id TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::InsertNewBranch(
			const Id& parentId,
			const QString& name,
			const QString& description,
			const Id& proposedElementId,
			const idoc::IDocumentMetaInfo* elementMetaInfoPtr)
{
	return m_collection.InsertNewBranch(
				parentId,
				name,
				description,
				proposedElementId,
				elementMetaInfoPtr);
}


template<class BaseInterface, class ObjectImpl>
inline QByteArray TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const Id& parentId)
{
	return m_collection.InsertNewObject(
				typeId,
				name,
				description,
				defaultValuePtr,
				proposedObjectId,
				dataMetaInfoPtr,
				collectionItemMetaInfoPtr,
				parentId);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::RemoveElement(const QByteArray& elementId)
{
	return m_collection.RemoveElement(elementId);
}


template<class BaseInterface, class ObjectImpl>
inline const istd::IChangeable* TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetObjectPtr(const QByteArray& objectId) const
{
	return m_collection.GetObjectPtr(objectId);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const
{
	return m_collection.GetObjectData(objectId, dataPtr);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode)
{
	return m_collection.SetObjectData(objectId, object, mode);
}


template<class BaseInterface, class ObjectImpl>
inline imtbase::IObjectCollection* TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			const imtbase::ICollectionInfo::Id& parentId,
			int iterationFlags) const
{
	return m_collection.CreateSubCollection(offset, count, selectionParamsPtr, parentId, iterationFlags);
}


template<class BaseInterface, class ObjectImpl>
imtbase::IObjectCollectionIterator* TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::CreateObjectCollectionIterator(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			const Id& parentId,
			int iterationFlags) const
{
	return m_collection.CreateObjectCollectionIterator(offset, count, selectionParamsPtr, parentId, iterationFlags);
}


// reimplemented (IObjectCollectionInfo)

template<class BaseInterface, class ObjectImpl>
inline const iprm::IOptionsList* TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetObjectTypesInfo() const
{
	return &m_types;
}


template<class BaseInterface, class ObjectImpl>
inline imtbase::ICollectionInfo::Id TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetObjectTypeId(const QByteArray& /*objectId*/) const
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
inline int TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetElementsCount(
			const iprm::IParamsSet* selectionParamPtr,
			const Id& parentId,
			int iterationFlags) const
{
	return m_collection.GetElementsCount(selectionParamPtr, parentId, iterationFlags);
}


template<class BaseInterface, class ObjectImpl>
inline imtbase::ICollectionInfo::Ids TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			const Id& parentId,
			int iterationFlags) const
{
	return m_collection.GetElementIds(offset, count, selectionParamsPtr, parentId, iterationFlags);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetSubsetInfo(
			imtbase::ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			const Id& /*parentId*/,
			int /*iterationFlags*/) const
{
	return false;
}


template<class BaseInterface, class ObjectImpl>
inline ICollectionInfo::Id TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetParentId(const Id& elementId) const
{
	return m_collection.GetParentId(elementId);
}


template<class BaseInterface, class ObjectImpl>
inline ICollectionInfo::Ids TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetElementPath(const Id& elementId) const
{
	return m_collection.GetElementPath(elementId);;
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::IsBranch(const Id& elementId) const
{
	return m_collection.IsBranch(elementId);
}


template<class BaseInterface, class ObjectImpl>
inline QVariant TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	return m_collection.GetElementInfo(elementId, infoType);
}


template<class BaseInterface, class ObjectImpl>
inline idoc::MetaInfoPtr TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetElementMetaInfo(const Id& elementId) const
{
	return m_collection.GetElementMetaInfo(elementId);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::SetElementName(const QByteArray& objectId, const QString& objectName)
{
	return m_collection.SetElementName(objectId, objectName);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::SetElementDescription(const QByteArray& objectId, const QString& objectDescription)
{
	return m_collection.SetElementDescription(objectId, objectDescription);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::SetElementEnabled(const QByteArray& objectId, bool isEnabled)
{
	return m_collection.SetElementEnabled(objectId, isEnabled);
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


