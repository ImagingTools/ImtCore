// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IUserInfo.h>
#include <imtauth/CUserBaseInfo.h>


namespace imtauth
{


class CUserInfo: virtual public IUserInfo, public CUserBaseInfo
{
public:
	typedef CUserBaseInfo BaseClass;

	// reimplemented (iser::IUserInfo)
	virtual QByteArray GetPasswordHash() const override;
	virtual void SetPasswordHash(const QByteArray& passwordHash) override;
	virtual QString GetMail() const override;
	virtual void SetMail(const QString& mail) override;
	virtual bool IsAdmin() const override;
	virtual IUserGroupInfo::GroupIds GetGroups() const override;
	virtual void AddToGroup(const QByteArray& groupId) override;
	virtual bool RemoveFromGroup(const QByteArray& groupId) override;
	virtual RoleIds GetRoles(const QByteArray& productId) const override;
	virtual FeatureIds GetPermissions(const QByteArray& productId = QByteArray()) const override;
	virtual SystemInfoList GetSystemInfos() const override;
	virtual bool AddToSystem(SystemInfo systemInfo) override;
	virtual bool RemoveFromSystem(const QByteArray& systemId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_passwordHash;
	QString m_mail;
	IUserGroupInfo::GroupIds m_groupIds;
	QDateTime m_lastConnection;
	SystemInfoList m_systemInfos;
};


typedef imtbase::TIdentifiableWrap<CUserInfo> CIdentifiableUserInfo;


} // namespace imtauth


