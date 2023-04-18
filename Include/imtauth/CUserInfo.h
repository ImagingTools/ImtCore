#pragma once


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
	virtual RoleIds GetRoles() const override;
	virtual FeatureIds GetPermissions() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_passwordHash;
	QString m_mail;
	IUserGroupInfo::GroupIds m_groupIds;
};


typedef imtbase::TIdentifiableWrap<CUserInfo> CIdentifiableUserInfo;


} // namespace imtauth



