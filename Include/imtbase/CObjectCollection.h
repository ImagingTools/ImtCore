#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QUuid>

// ACF includes
#include <istd/TComposedFactory.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Implementation of a general object collection.
	\ingroup Collection
*/
class CObjectCollection:
			virtual public IObjectCollection,
			public istd::TComposedFactory<istd::IChangeable>
{
public:
	typedef istd::TComposedFactory<istd::IChangeable> BaseClass;

	CObjectCollection();
	virtual ~CObjectCollection();

	// reimplemented (IObjectCollection)
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr) override;
	virtual istd::IChangeable* GetEditableObject(const QByteArray& objectId) const override;
	virtual bool RemoveObject(const QByteArray& objectId) override;
	virtual void SetObjectName(const QByteArray& objectId, const QString& objectName) override;
	virtual void SetObjectDescription(const QByteArray& objectId, const QString& objectDescription) override;
	virtual void SetObjectEnabled(const QByteArray& objectId, bool isEnabled = true) override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;

	// reimplemented (IObjectProvider)
	virtual const istd::IChangeable* GetDataObject(const QByteArray & objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	virtual istd::IChangeable* CreateObjectInstance(const QByteArray& typeId) const;

protected:
	typedef istd::TOptDelPtr<istd::IChangeable> ObjectPtr;

	struct ObjectInfo
	{
		ObjectInfo()
			:isEnabled(true),
			flags(OF_DEFAULT)
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

	imod::CModelUpdateBridge m_modelUpdateBridge;

	Objects m_objects;
};


} // namespace imtbase


