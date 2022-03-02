#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QUuid>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <imod/CModelUpdateBridge.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/ICollectionDataController.h>


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
	virtual bool ExportFile(const imtbase::IObjectCollection& collection, const QByteArray& objectId, const QString& targetFilePath = QString()) const override;
	virtual QByteArray ImportFile(imtbase::IObjectCollection& collection, const QByteArray& typeId, const QString& sourceFilePath = QString()) const override;

	// reimplemented (IObjectCollection)
	virtual const IRevisionController* GetRevisionController() const override;
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
	virtual bool RegisterEventHandler(IObjectCollectionEventHandler* eventHandler) override;
	virtual bool UnregisterEventHandler(IObjectCollectionEventHandler* eventHandler) override;

	// reimplemented (IObjectCollectionInfo)
	virtual bool GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual Ids GetElementIds(const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

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
			flags(OF_ALL)
		{
			id = QUuid::createUuid().toByteArray();
		}

		ObjectInfo(const ObjectInfo& object)
		{
			this->objectPtr = object.objectPtr;

			this->id = object.id;
			this->typeId = object.typeId;
			this->name = object.name;
			this->isEnabled = object.isEnabled;
			this->flags = object.flags;
			this->description = object.description;
			this->metaInfo.CopyFrom(object.metaInfo);
			this->contentsMetaInfoPtr = object.contentsMetaInfoPtr;
		}

		bool isEnabled;
		DataPtr objectPtr;
		QString name;
		QString description;
		QByteArray id;
		QByteArray typeId;
		int flags;
		idoc::CStandardDocumentMetaInfo metaInfo;

		/**
			Meta-informations for the data object.
		*/
		imtbase::IMetaInfoCreator::MetaInfoPtr contentsMetaInfoPtr;
	};

	typedef QVector<ObjectInfo> Objects;
	typedef QList<IObjectCollectionEventHandler*> EventHandlerList;

	Objects m_objects;

protected:
	// abstract methods
	virtual DataPtr CreateObjectInstance(const QByteArray& typeId) const = 0;

protected:
	virtual bool InsertObjectIntoCollection(ObjectInfo info);
	virtual int GetItemDefaultFlags() const;
	ObjectInfo* GetObjectInfo(const QByteArray& id) const;
	virtual void RemoveAllObjects();

private:
	imod::CModelUpdateBridge m_modelUpdateBridge;

	EventHandlerList m_eventHandlerList;
};


} // namespace imtbase


