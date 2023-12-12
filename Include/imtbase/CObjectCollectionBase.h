#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QUuid>
#include <QtCore/QReadWriteLock>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <imod/CModelUpdateBridge.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/CObjectCollectionMetaInfo.h>
#include <imtbase/ICollectionDataController.h>
#include <imtbase/IMetaInfoCreator.h>
#include <imtbase/IObjectCollection.h>


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
				const QString& sourceFilePath = QString()) const override;

	// reimplemented (IObjectCollection)
	virtual const IRevisionController* GetRevisionController() const override;
	virtual const ICollectionDataController* GetDataController() const override;
	virtual int GetOperationFlags(const Id& objectId = Id()) const override;
	virtual Id InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				DataPtr defaultValuePtr = DataPtr(),
				const Id& proposedElementId = Id(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveElement(const Id& elementId, const IOperationContext* operationContextPtr = nullptr) override;
	virtual const istd::IChangeable* GetObjectPtr(const Id& objectId) const override;
	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr) const override;
	virtual bool SetObjectData(const Id& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS, const IOperationContext* operationContextPtr = nullptr) override;
	virtual IObjectCollection* CreateSubCollection(int offset, int count, const iprm::IParamsSet *selectionParamsPtr) const override;
	virtual imtbase::IObjectCollectionIterator* CreateObjectCollectionIterator(
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

	// (reimplemented from ISerializable)
	virtual bool Serialize(iser::IArchive & archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
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
			this->objectPtr = object.objectPtr;
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
		DataPtr objectPtr;
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
	virtual DataPtr CreateObjectInstance(const QByteArray& typeId) const = 0;

protected:
	virtual bool InsertObjectIntoCollection(ObjectInfo info);
	virtual int GetItemDefaultFlags() const;
	ObjectInfo* GetObjectInfo(const Id& id) const;
	virtual void RemoveAllObjects();


protected:
	imod::CModelUpdateBridge m_modelUpdateBridge;
	mutable QReadWriteLock m_lock;
};


} // namespace imtbase


