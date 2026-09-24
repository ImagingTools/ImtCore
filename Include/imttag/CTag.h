// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imttag/ITag.h>


namespace imttag
{


class CTag: virtual public ITag
{
public:
	CTag();

	// reimplemented (imttag::ITag)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QString GetName() const override;
	virtual void SetName(const QString& name) override;
	virtual QString GetColor() const override;
	virtual void SetColor(const QString& color) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual bool IsSystem() const override;
	virtual void SetSystem(bool isSystem) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_id;
	QString m_name;
	QString m_color;
	QString m_description;
	bool m_isSystem;
};


typedef imtbase::TIdentifiableWrap<CTag> CIdentifiableTag;


} // namespace imttag


