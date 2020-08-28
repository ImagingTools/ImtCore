#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QUuid>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Basic implementation of a general data object collection.
	\ingroup Collection
*/
class CObjectCollectionBase: virtual public IObjectCollection
{
public:
	CObjectCollectionBase();
	virtual ~CObjectCollectionBase();

	// reimplemented (IObjectCollection)
	virtual const IRevisionController* GetRevisionController() const override;
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual bool GetDataMetaInfo(const QByteArray& objectId, MetaInfoPtr& metaInfoPtr) const override;
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const QByteArray& proposedObjectId = QByteArray()) override;
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
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	typedef istd::TOptDelPtr<istd::IChangeable> ObjectPtr;

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
			this->objectPtr.TakeOver(const_cast<ObjectInfo&>(object).objectPtr);

			this->id = object.id;
			this->typeId = object.typeId;
			this->name = object.name;
			this->isEnabled = object.isEnabled;
			this->flags = object.flags;
			this->description = object.description;
		}

		bool isEnabled;
		ObjectPtr objectPtr;
		QString name;
		QString description;
		QByteArray id;
		QByteArray typeId;
		int flags;
	};

	typedef QVector<ObjectInfo> Objects;
	typedef QList<IObjectCollectionEventHandler*> EventHandlerList;

protected:
	virtual istd::IChangeable* CreateObjectInstance(const QByteArray& typeId) const = 0;
	virtual bool InsertObjectIntoCollection(const ObjectInfo& info);
	virtual int GetItemDefaultFlags() const;
	ObjectInfo* GetObjectInfo(const QByteArray& id) const;

private:
	imod::CModelUpdateBridge m_modelUpdateBridge;

	Objects m_objects;
	EventHandlerList m_eventHandlerList;
};


} // namespace imtbase


