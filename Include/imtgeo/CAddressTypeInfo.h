// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgeo/IAddressTypeInfo.h>


namespace imtgeo
{


class CAddressTypeInfo: virtual public IAddressTypeInfo
{
public:
	// reimplemented (IAddressTypeInfo)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;

	virtual QString GetName() const override;
	virtual void SetName(const QString& name) override;

	virtual QString GetShortName() const override;
	virtual void SetShortName(const QString& sname) override;

	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& des) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_id;
	QString m_name;
	QString m_shortName;
	QString m_description;
};


} // namespace imtgeo

