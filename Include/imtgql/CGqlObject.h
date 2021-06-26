#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QVariant>

// ImtCore includes
#include <istd/TSmartPtr.h>


namespace imtgql
{


class CGqlObject
{
public:
	CGqlObject(const QByteArray& objectId = QByteArray());

	QByteArray GetId() const;
	QByteArrayList GetFieldIds() const;
	QVariant GetFieldArgumentValue(const QByteArray& fieldId) const;
	const CGqlObject *GetFieldArgumentObjectPtr(const QByteArray& fieldId) const;
	void InsertField(const QByteArray& fieldId);
	void InsertFieldArgument(const QByteArray& fieldId, const QVariant& value);
	void InsertFieldObject(CGqlObject *objectPtr);
	bool IsObject(const QByteArray& fieldId) const;

protected:
	struct Field
	{
		QByteArray id;
		QVariant value;
		istd::TSmartPtr<CGqlObject> objectPtr;
	};

	typedef QMap<QByteArray, Field> Fields;
	Fields m_fieldsMap;
	QByteArray m_objectId;
};


} // namespace imtgql


