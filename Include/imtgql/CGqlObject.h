#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QVariant>

// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <istd/TSmartPtr.h>
#include <imtgql/CGqlEnum.h>


namespace imtgql
{

class IGqlField: virtual public iser::IObject
{
public:
	virtual QByteArray GetId() const = 0;
	virtual QByteArrayList GetFieldIds() const  = 0;
	virtual IGqlField* GetParentObject() const  = 0;
	virtual void InsertField(const QByteArray& fieldId)  = 0;
	virtual void InsertSubQuery(const IGqlField& object)  = 0;
	virtual bool IsObject(const QByteArray& fieldId) const  = 0;
};


class IGqlDataObject: virtual public iser::IObject
{
public:
	virtual QByteArray GetId() const = 0;
	virtual QByteArrayList GetFieldIds() const = 0;
	virtual QVariant GetFieldArgumentValue(const QByteArray& fieldId) const = 0;
	virtual IGqlDataObject *CreateFieldObject(const QByteArray& fieldId) = 0;
	virtual const IGqlDataObject *GetFieldArgumentObjectPtr(const QByteArray& fieldId, int index = 0) const = 0;
	virtual int GetObjectsCount(const QByteArray& fieldId) const = 0;
	virtual QList<const IGqlDataObject*> GetFieldArgumentObjectPtrList(const QByteArray& fieldId) const = 0;
	virtual IGqlDataObject* GetParentObject() const = 0;
	virtual void InsertData(const QByteArray& fieldId, const QVariant& value) = 0;
	virtual void InsertData(const QByteArray& fieldId, const imtgql::CGqlEnum& value) = 0;
	virtual void InsertData(const QByteArray& fieldId, const IGqlDataObject& object) = 0;
	virtual void InsertData(const QByteArray& fieldId, const QList<IGqlDataObject> objectList) = 0;
	virtual bool IsObject(const QByteArray& fieldId) const = 0;
	virtual bool IsEnum(const QByteArray& fieldId) const = 0;
	virtual bool IsObjectList(const QByteArray& fieldId) const = 0;
};


class CGqlFieldObject: virtual public iser::IObject
{
public:
	CGqlFieldObject(const QByteArray& objectId = QByteArray());

	QByteArray GetId() const;
	QByteArrayList GetFieldIds() const;
	CGqlFieldObject* GetParentObject() const;
	void InsertField(const QByteArray& fieldId);
	void InsertFieldObject(const CGqlFieldObject& object);
	bool IsObject(const QByteArray& fieldId) const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;


protected:
	QList<QByteArray> m_emptyFields;
	QMap<QByteArray, istd::TSmartPtr<CGqlFieldObject>> m_fieldObjects;
	QByteArray m_objectId;
	CGqlFieldObject* m_parentPtr;
};



class CGqlDataObject: virtual public iser::IObject
{
public:
	CGqlDataObject(const QByteArray& objectId = QByteArray());

	QByteArray GetId() const;
	QByteArrayList GetFieldIds() const;
	QVariant GetFieldArgumentValue(const QByteArray& fieldId) const;
	CGqlDataObject *CreateFieldObject(const QByteArray& fieldId);
	const CGqlDataObject *GetFieldArgumentObjectPtr(const QByteArray& fieldId, int index = 0) const;
	int GetObjectsCount(const QByteArray& fieldId) const;
	QList<const CGqlDataObject*> GetFieldArgumentObjectPtrList(const QByteArray& fieldId) const;
	CGqlDataObject* GetParentObject() const;
	void InsertData(const QByteArray& fieldId, const QVariant& value);
	void InsertData(const QByteArray& fieldId, const imtgql::CGqlEnum& value);
	void InsertData(const QByteArray& fieldId, const CGqlDataObject& object);
	void InsertData(const QByteArray& fieldId, const QList<CGqlDataObject> objectList);
	bool IsObject(const QByteArray& fieldId) const;
	bool IsEnum(const QByteArray& fieldId) const;
	bool IsObjectList(const QByteArray& fieldId) const;

	// comfort
	Q_REQUIRED_RESULT QVariant operator[](const QByteArray& fieldId) const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	void InsertFieldObject(istd::TSmartPtr<CGqlDataObject> objectPtr);
	void RemoveField(const QByteArray& fieldId);

protected:
	QList<QByteArray> m_emptyFields;
	QMap<QByteArray, QVariant> m_simpleFields;
	QMap<QByteArray, istd::TSmartPtr<CGqlDataObject>> m_objectFields;
	QMap<QByteArray, QList<istd::TSmartPtr<CGqlDataObject>>> m_objectFieldsArray;
	QByteArray m_objectId;
	CGqlDataObject* m_parentPtr;
};



class CGqlObject: virtual public iser::IObject
{
public:
	CGqlObject(const QByteArray& objectId = QByteArray());

	QByteArray GetId() const;
	QByteArrayList GetFieldIds() const;
	QVariant GetFieldArgumentValue(const QByteArray& fieldId) const;
	CGqlObject *CreateFieldObject(const QByteArray& fieldId);
	const CGqlObject *GetFieldArgumentObjectPtr(const QByteArray& fieldId, int index = 0) const;
	int GetObjectsCount(const QByteArray& fieldId) const;
	QList<const CGqlObject*> GetFieldArgumentObjectPtrList(const QByteArray& fieldId) const;
	CGqlObject* GetParentObject() const;
	void InsertField(const QByteArray& fieldId);
	void InsertField(const QByteArray& fieldId, const QVariant& value);
	void InsertField(const QByteArray& fieldId, const imtgql::CGqlEnum& value);
	void InsertField(const QByteArray& fieldId, const CGqlObject& object);
	void InsertField(const QByteArray& fieldId, const QList<CGqlObject> objectList);
	bool IsObject(const QByteArray& fieldId) const;
	bool IsEnum(const QByteArray& fieldId) const;
	bool IsObjectList(const QByteArray& fieldId) const;

	// comfort
	Q_REQUIRED_RESULT QVariant operator[](const QByteArray& fieldId) const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	void InsertFieldObject(istd::TSmartPtr<CGqlObject> objectPtr);
	void RemoveField(const QByteArray& fieldId);

protected:
	QList<QByteArray> m_emptyFields;
	QMap<QByteArray, QVariant> m_simpleFields;
	QMap<QByteArray, istd::TSmartPtr<CGqlObject>> m_objectFields;
	QMap<QByteArray, QList<istd::TSmartPtr<CGqlObject>>> m_objectFieldsArray;
	QByteArray m_objectId;
	CGqlObject* m_parentPtr;
};


// inline methods
inline QVariant CGqlObject::operator[](const QByteArray& fieldId) const
{
	return GetFieldArgumentValue(fieldId);
}


} // namespace imtgql


