// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <iser/IObject.h>

// ImtCore includes
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
	const CGqlParamObject* GetParamArgumentObjectPtr(const QByteArray& paramId, qsizetype index = 0) const;
	qsizetype GetObjectsCount(const QByteArray& paramId) const;
	QList<const CGqlParamObject*> GetParamArgumentObjectPtrList(const QByteArray& paramId) const;
	CGqlParamObject* GetParentObject() const;
	void InsertParam(const QByteArray& paramId, const QVariant& value);
	void InsertParam(const QByteArray& paramId, const imtgql::CGqlEnum& value);
	void InsertParam(const QByteArray& paramId, const CGqlParamObject& object);
	void InsertParam(const QByteArray& paramId, const QList<CGqlParamObject>& objectList);
	CGqlParamObject* AppendParamToArray(const QByteArray& paramId, const CGqlParamObject& object);
	bool IsObject(const QByteArray& paramId) const;
	bool IsEnum(const QByteArray& paramId) const;
	bool IsObjectList(const QByteArray& paramId) const;

	// comfort
	Q_REQUIRED_RESULT QVariant operator[](const QByteArray& paramId) const;
	Q_REQUIRED_RESULT bool ContainsParam(const QByteArray& paramId) const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	void RemoveParam(const QByteArray& paramId);

protected:
	QMap<QByteArray, QVariant> m_simpleParams;
	QMap<QByteArray, istd::TSharedInterfacePtr<CGqlParamObject>> m_objectParams;
	QMap<QByteArray, QList<istd::TSharedInterfacePtr<CGqlParamObject>>> m_objectParamsArray;
	QByteArray m_objectId;
	CGqlParamObject* m_parentPtr;
};


// inline methods
inline QVariant CGqlParamObject::operator[](const QByteArray& paramId) const
{
	return GetParamArgumentValue(paramId);
}


inline bool CGqlParamObject::ContainsParam(const QByteArray& paramId) const
{
	if (m_simpleParams.contains(paramId)){
		return true;
	}
	if (m_objectParams.contains(paramId)){
		return true;
	}
	if (m_objectParamsArray.contains(paramId)){
		return true;
	}

	return false;
}


} // namespace imtgql


Q_DECLARE_METATYPE(const imtgql::CGqlParamObject*)


