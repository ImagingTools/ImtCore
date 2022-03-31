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
	virtual bool GetDataMetaInfo(const QByteArray& objectId, MetaInfoPtr& metaInfoPtr) const override;
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				DataPtr defaultValuePtr = DataPtr(),
				const QByteArray& proposedObjectId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr) override;
	virtual bool RemoveObject(const QByteArray& objectId) override;
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;
	virtual bool GetObjectData( const QByteArray& objectId, DataPtr& dataPtr) const override;
	virtual bool SetObjectData( const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual void SetObjectName(const QByteArray& objectId, const QString& objectName) override;
	virtual void SetObjectDescription(const QByteArray& objectId, const QString& objectDescription) override;
	virtual void SetObjectEnabled(const QByteArray& objectId, bool isEnabled = true) override;
	virtual bool RegisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler) override;
	virtual bool UnregisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler) override;

	// reimplemented (IObjectCollectionInfo)
	virtual bool GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual int GetElementsCount(const iprm::IParamsSet* selectionParamPtr = nullptr) const override;
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

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

	m_collection.RegisterFactory(new istd::TSingleFactory<istd::IChangeable, imod::TModelWrap<ObjectImpl>>(typeId), true);

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
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetDataMetaInfo(const QByteArray& objectId, MetaInfoPtr& metaInfoPtr) const
{
	return m_collection.GetDataMetaInfo(objectId, metaInfoPtr);
}


template<class BaseInterface, class ObjectImpl>
inline QByteArray TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr)
{
	return m_collection.InsertNewObject(typeId, name, description, defaultValuePtr, proposedObjectId, dataMetaInfoPtr, collectionItemMetaInfoPtr);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::RemoveObject(const QByteArray& objectId)
{
	return m_collection.RemoveObject(objectId);
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
inline void TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::SetObjectName(const QByteArray& objectId, const QString& objectName)
{
	m_collection.SetObjectName(objectId, objectName);
}


template<class BaseInterface, class ObjectImpl>
inline void TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::SetObjectDescription(const QByteArray& objectId, const QString& objectDescription)
{
	m_collection.SetObjectDescription(objectId, objectDescription);
}


template<class BaseInterface, class ObjectImpl>
inline void TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::SetObjectEnabled(const QByteArray& objectId, bool isEnabled)
{
	m_collection.SetObjectEnabled(objectId, isEnabled);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::RegisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler)
{
	return m_collection.RegisterEventHandler(eventHandler);
}


template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::UnregisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler)
{
	return m_collection.UnregisterEventHandler(eventHandler);
}


// reimplemented (IObjectCollectionInfo)

template<class BaseInterface, class ObjectImpl>
inline bool TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const
{
	return m_collection.GetCollectionItemMetaInfo(objectId, metaInfo);
}

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


// reimplemented (ICollectionInfo)

template<class BaseInterface, class ObjectImpl>
inline int TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetElementsCount(const iprm::IParamsSet* selectionParamPtr) const
{
	return m_collection.GetElementsCount(selectionParamPtr);
}


template<class BaseInterface, class ObjectImpl>
inline imtbase::ICollectionInfo::Ids TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	return m_collection.GetElementIds(offset, count, selectionParamsPtr);
}


template<class BaseInterface, class ObjectImpl>
inline QVariant TAggergatedObjectCollectionWrap<BaseInterface, ObjectImpl>::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	return m_collection.GetElementInfo(elementId, infoType);
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


