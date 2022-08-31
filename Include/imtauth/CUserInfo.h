#pragma once


// ImtCore includes
#include "imtbase/CCollectionInfo.h"
#include <imtauth/IUserInfo.h>
#include <imtauth/CRole.h>


namespace imtauth
{

class CUserInfo: virtual public IUserInfo
{

public:
	typedef CRole BaseClass;

	// reimplemented (iser::IUserInfo)
    virtual const imtlic::IFeatureInfoProvider* GetPermissionProvider() const override;
    virtual const IRoleInfoProvider *GetRoleProvider() const override;
	virtual QByteArray GetId() const override;
	virtual void SetId(QByteArray id) override;
	virtual QString GetUsername() const override;
	virtual void SetUsername(QString username) override;
	virtual QString GetName() const override;
	virtual void SetName(QString name) override;
	virtual QByteArray GetPasswordHash() const override;
	virtual void SetPasswordHash(QByteArray passwordHash) override;
	virtual QString GetMail() const override;
	virtual void SetMail(QString mail) override;
    virtual FeatureIds GetPermissions() const override;
    virtual FeatureIds GetLocalPermissions() const override;
    virtual void SetLocalPermissions(const FeatureIds &permissions) override;
    virtual FeatureIds GetProhibitions() const override;
    virtual void SetProhibitions(const FeatureIds &prohibitions) override;
    virtual RoleIds GetRoles() const override;
    virtual void SetRoles(const RoleIds &roles) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
    const imtlic::IFeatureInfoProvider* m_permissionProviderPtr;
    const imtauth::IRoleInfoProvider* m_roleProviderPtr;

private:
	QByteArray m_userId;
	QString m_username;
	QString m_name;
	QByteArray m_passwordHash;
	QString m_mail;   
    IUserInfo::FeatureIds m_userPermissions;
    IUserInfo::FeatureIds m_userRestrictions;
    IUserInfo::RoleIds m_userRoles; 
};


} // namespace imtauth



