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


class CGqlParamObject: virtual public iser::IObject
{
public:
	CGqlParamObject();

	QByteArrayList GetFieldIds() const;
	QVariant GetFieldArgumentValue(const QByteArray& fieldId) const;
	CGqlParamObject* CreateFieldObject(const QByteArray& fieldId);
	const CGqlParamObject* GetFieldArgumentObjectPtr(const QByteArray& fieldId, int index = 0) const;
	int GetObjectsCount(const QByteArray& fieldId) const;
	QList<const CGqlParamObject*> GetFieldArgumentObjectPtrList(const QByteArray& fieldId) const;
	CGqlParamObject* GetParentObject() const;
	void InsertField(const QByteArray& fieldId, const QVariant& value);
	void InsertField(const QByteArray& fieldId, const imtgql::CGqlEnum& value);
	void InsertField(const QByteArray& fieldId, const CGqlParamObject& object);
	void InsertField(const QByteArray& fieldId, const QList<CGqlParamObject> objectList);
	CGqlParamObject* AppendFieldToArray(const QByteArray& fieldId, const CGqlParamObject& object);
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
	void InsertFieldObject(istd::TSmartPtr<CGqlParamObject> objectPtr);
	void RemoveField(const QByteArray& fieldId);

protected:
	QMap<QByteArray, QVariant> m_simpleFields;
	QMap<QByteArray, istd::TSmartPtr<CGqlParamObject>> m_objectFields;
	QMap<QByteArray, QList<istd::TSmartPtr<CGqlParamObject>>> m_objectFieldsArray;
	QByteArray m_objectId;
	CGqlParamObject* m_parentPtr;
};


// inline methods
inline QVariant CGqlParamObject::operator[](const QByteArray& fieldId) const
{
	return GetFieldArgumentValue(fieldId);
}


} // namespace imtgql


Q_DECLARE_METATYPE(const imtgql::CGqlParamObject*)


