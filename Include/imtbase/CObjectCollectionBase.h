// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QUuid>
#include <QtCore/QReadWriteLock>

// ACF includes
#include <istd/TOptInterfacePtr.h>
#include <imod/CModelUpdateBridge.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imod/CSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/CObjectCollectionMetaInfo.h>
#include <imtbase/ICollectionDataController.h>
#include <imtbase/IMetaInfoCreator.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionIterator.h>


namespace imtbase
{


/**
	Basic implementation of a general data object collection.
	\ingroup Collection
*/
class CObjectCollectionBase:
			virtual public IObjectCollection,
			virtual protected ICollectionDataController,
			virtual public iser::ISerializable
{
public:
	CObjectCollectionBase();
	virtual ~CObjectCollectionBase();

	// reimplemented (ICollectionDataController)
	virtual const ifile::IFilePersistence* GetPersistenceForObjectType(const QByteArray& typeId) const override;
	virtual bool ExportFile(
				const imtbase::IObjectCollection& collection,
				const Id& objectId,
				const QString& targetFilePath = QString()) const override;
	virtual Id ImportFile(
				imtbase::IObjectCollection& collection,
				const QByteArray& typeId,
				const QString& sourceFilePath = QString(),
				const QString& objectName = QString()) const override;

	// reimplemented (IObjectCollection)
	virtual const IRevisionController* GetRevisionController() const override;
	virtual const ICollectionDataController* GetDataController() const override;
	virtual int GetOperationFlags(const Id& objectId = Id()) const override;
	virtual Id InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const Id& proposedElementId = Id(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
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
	virtual const istd::IChangeable* GetObjectPtr(const Id& objectId) const override;
	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
	virtual bool SetObjectData(
				const Id& objectId,
				const istd::IChangeable& object,
				CompatibilityMode mode = CM_WITHOUT_REFS,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual IObjectCollectionUniquePtr CreateSubCollection(int offset, int count, const iprm::IParamsSet *selectionParamsPtr) const override;
	virtual imtbase::IObjectCollectionIterator* CreateObjectCollectionIterator(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual QByteArray GetObjectTypeId(const Id& objectId) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual int GetElementsCount(
				const iprm::IParamsSet* selectionParamPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
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
	virtual QVariant GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true, ilog::IMessageConsumer* logPtr = nullptr) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive & archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	struct CollectionIterator : public imod::CSingleModelObserverBase, virtual public imtbase::IObjectCollectionIterator
	{
	public:
		CollectionIterator() = delete;
		CollectionIterator(const CObjectCollectionBase& parent);
		~CollectionIterator();

		void Reset();
		bool IsValid() const;

		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

		// reimplemented (imtbase::IObjectCollectionIterator)
		virtual bool Next() const override;
		virtual bool Previous() const override;
		virtual QByteArray GetObjectId() const override;
		virtual QByteArray GetObjectTypeId() const override;
		virtual bool GetObjectData(IObjectCollection::DataPtr& dataPtr) const override;
		virtual idoc::MetaInfoPtr GetDataMetaInfo() const override;
		virtual idoc::MetaInfoPtr GetCollectionMetaInfo() const override;
		virtual QVariant GetElementInfo(int infoType) const override;
		virtual QVariant GetElementInfo(QByteArray infoId) const override;

	private:
		const IObjectCollection* GetCollectionPtr(int index) const;
		QByteArray GetObjectId(int index) const;

	private:
		const CObjectCollectionBase& m_parent;
		mutable int m_index;
	};

	struct ObjectInfo
	{
		ObjectInfo()
			:isEnabled(true),
			flags(OF_ALL),
			copyMode(istd::IChangeable::CM_WITHOUT_REFS)
		{
			id = QUuid::createUuid().toByteArray();
		}

		ObjectInfo(const ObjectInfo& object)
		{
			this->dataPtr = object.dataPtr;
			this->copyMode = object.copyMode;

			this->id = object.id;
			this->typeId = object.typeId;
			this->name = object.name;
			this->isEnabled = object.isEnabled;
			this->flags = object.flags;
			this->description = object.description;
			this->collectionItemMetaInfo.CopyFrom(object.collectionItemMetaInfo);
			this->contentsMetaInfoPtr = object.contentsMetaInfoPtr;
		}

		bool isEnabled;
		istd::TOptInterfacePtr<IChangeable> dataPtr;
		QString name;
		QString description;
		QByteArray id;
		QByteArray typeId;
		int flags;
		istd::IChangeable::CompatibilityMode copyMode;
		imtbase::CObjectCollectionMetaInfo collectionItemMetaInfo;

		/**
			Meta-informations for the data object.
		*/
		idoc::MetaInfoPtr contentsMetaInfoPtr;
	};

	typedef QVector<ObjectInfo> Objects;

	Objects m_objects;

protected:
	// abstract methods
	
	/**
		Create object instance of the given type.
	*/
	virtual istd::IChangeableUniquePtr CreateObjectInstance(const QByteArray& typeId) const = 0;

protected:
	/**
		Get an external object storage to persist the object of the given type.
		If the implementation provides such a collection,
		then the object will be stored there and this collection will only store the link.
	*/
	virtual IObjectCollection* GetObjectStorage(const QByteArray& typeId, const istd::IChangeable* objectPtr) const;

	/**
		Create sub-collection instance.
	*/
	virtual IObjectCollection* CreateSubCollectionInstance() const;

	virtual bool InsertObjectIntoCollection(ObjectInfo info);
	virtual int GetItemDefaultFlags() const;
	ObjectInfo* GetObjectInfo(const Id& id) const;
	virtual void RemoveAllObjects();

private:
	istd::IChangeableUniquePtr CreateDataObject(const QByteArray& typeId) const;

protected:
	imod::CModelUpdateBridge m_modelUpdateBridge;
	mutable QReadWriteLock m_lock;
};


} // namespace imtbase


