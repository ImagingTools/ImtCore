// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IPartyRoleInfo.h>


namespace imtauth
{


class CPartyRoleInfo: virtual public IPartyRoleInfo
{
public:
	CPartyRoleInfo();

	// reimplemented (imtauth::IPartyRoleInfo)
	virtual QByteArray GetRoleId() const override;
	virtual void SetRoleId(const QByteArray& value) override;
	virtual QString GetName() const override;
	virtual void SetName(const QString& value) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& value) override;
	virtual QByteArray GetOwnerTenantId() const override;
	virtual void SetOwnerTenantId(const QByteArray& value) override;
	virtual int GetApplicability() const override;
	virtual void SetApplicability(int value) override;
	virtual QByteArray GetPairedRoleId() const override;
	virtual void SetPairedRoleId(const QByteArray& value) override;
	virtual QByteArray GetTemplateProductId() const override;
	virtual void SetTemplateProductId(const QByteArray& value) override;
	virtual QByteArrayList GetTemplatePermissionIds() const override;
	virtual void SetTemplatePermissionIds(const QByteArrayList& value) override;
	virtual bool IsArchived() const override;
	virtual void SetArchived(bool value) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_roleId;
	QString m_name;
	QString m_description;
	QByteArray m_ownerTenantId;
	int m_applicability;
	QByteArray m_pairedRoleId;
	QByteArray m_templateProductId;
	QByteArrayList m_templatePermissionIds;
	bool m_archived;
};


typedef imtbase::TIdentifiableWrap<CPartyRoleInfo> CIdentifiablePartyRoleInfo;


} // namespace imtauth


