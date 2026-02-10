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


class CGqlFieldFragment;


class CGqlFieldObject: virtual public iser::IObject
{
public:
	CGqlFieldObject();

	QByteArrayList GetFieldIds() const;
	CGqlFieldObject* CreateFieldObject(const QByteArray& fieldId);
	CGqlFieldFragment* CreateFragmentObject(const QByteArray& typeId);
	const CGqlFieldObject* GetFieldArgumentObjectPtr(const QByteArray& fieldId) const;
	const CGqlFieldFragment* GetFragmentArgumentObjectPtr(const QByteArray& fieldId) const;
	CGqlFieldObject* GetParentObject() const;
	void InsertField(const QByteArray& fieldId);
	void InsertField(const QByteArray& fieldId, const CGqlFieldObject& object);
	void InsertFragment(const QByteArray& typeId, const CGqlFieldFragment& object);
	bool IsObject(const QByteArray& fieldId) const;
	bool IsFragment(const QByteArray& fieldId) const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;
protected:
	void RemoveField(const QByteArray& fieldId);

protected:
	QList<QByteArray> m_emptyFields;
	QMap<QByteArray, istd::TSharedInterfacePtr<CGqlFieldObject>> m_objectFields; // map <fieldId, smart<CGqlFieldObject>>
	QMap<QByteArray,  istd::TSharedInterfacePtr<CGqlFieldFragment>> m_fragmentFields; // map <typeId, smart<FieldFragment>>
	CGqlFieldObject* m_parentPtr;
};


} // namespace imtgql


Q_DECLARE_METATYPE(const imtgql::CGqlFieldObject*)


