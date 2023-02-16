#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QVariant>

// ImtCore includes
#include <istd/TSmartPtr.h>
#include <imtgql/CGqlEnum.h>


namespace imtgql
{


class CGqlObject
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
	void InsertField(const QByteArray& fieldId, const QVariantList& value);
	void InsertField(const QByteArray& fieldId, const imtgql::CGqlEnum& value);
	void InsertField(const QByteArray& fieldId, const CGqlObject& object);
	void InsertField(const QByteArray& fieldId, const QList<CGqlObject> objectList);
	bool IsObject(const QByteArray& fieldId) const;
	bool IsEnum(const QByteArray& fieldId) const;
	bool IsObjectList(const QByteArray& fieldId) const;

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


} // namespace imtgql


