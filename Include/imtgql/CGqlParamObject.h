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

	QByteArrayList GetParamIds() const;
	QVariant GetParamArgumentValue(const QByteArray& paramId) const;
	CGqlParamObject* CreateParamObject(const QByteArray& paramId);
	const CGqlParamObject* GetParamArgumentObjectPtr(const QByteArray& paramId, int index = 0) const;
	int GetObjectsCount(const QByteArray& paramId) const;
	QList<const CGqlParamObject*> GetParamArgumentObjectPtrList(const QByteArray& paramId) const;
	CGqlParamObject* GetParentObject() const;
	void InsertParam(const QByteArray& paramId, const QVariant& value);
	void InsertParam(const QByteArray& paramId, const imtgql::CGqlEnum& value);
	void InsertParam(const QByteArray& paramId, const CGqlParamObject& object);
	void InsertParam(const QByteArray& paramId, const QList<CGqlParamObject> objectList);
	CGqlParamObject* AppendParamToArray(const QByteArray& paramId, const CGqlParamObject& object);
	bool IsObject(const QByteArray& paramId) const;
	bool IsEnum(const QByteArray& paramId) const;
	bool IsObjectList(const QByteArray& paramId) const;

	// comfort
	Q_REQUIRED_RESULT QVariant operator[](const QByteArray& paramId) const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	void RemoveParam(const QByteArray& paramId);

protected:
	QMap<QByteArray, QVariant> m_simpleParams;
	QMap<QByteArray, istd::TSmartPtr<CGqlParamObject>> m_objectParams;
	QMap<QByteArray, QList<istd::TSmartPtr<CGqlParamObject>>> m_objectParamsArray;
	QByteArray m_objectId;
	CGqlParamObject* m_parentPtr;
};


// inline methods
inline QVariant CGqlParamObject::operator[](const QByteArray& paramId) const
{
	return GetParamArgumentValue(paramId);
}


} // namespace imtgql


Q_DECLARE_METATYPE(const imtgql::CGqlParamObject*)


