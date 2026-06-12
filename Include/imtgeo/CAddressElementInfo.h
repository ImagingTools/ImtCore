// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgeo/IAddressElementInfo.h>
#include <imtgeo/CPosition.h>


namespace imtgeo
{


class CAddressElementInfo: public CPositionIdentifiable, virtual public IAddressElementInfo
{
public:
	using BaseClass = CPositionIdentifiable;

	// reimplemented (imtgeo::IAddressElementInfo)
	virtual QList<QByteArray> GetParentIds() const override;
	virtual void SetParentIds(const QList<QByteArray>& parentIds) override;

	virtual QByteArray GetAddressTypeId() const override;
	virtual void SetAddressTypeId(const QByteArray& typeId) override;

	virtual QString GetName() const override;
	virtual void SetName(const QString& name) override;

	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString &description) override;

	virtual QString GetFullAddress() const override;
	virtual void SetFullAddress(const QString& adr) override;

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
	QByteArray m_typeId;
	QString m_name;
	QString m_description;
	QString m_fullAddress;
	QByteArrayList m_parentIds;
};


} // namespace imtgeo



