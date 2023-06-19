#pragma once


// ImtCore includes
#include <imtauth/IUserBaseInfo.h>
#include "imtbase/CCollectionInfo.h"
#include <imtauth/CRole.h>


namespace imtauth
{


class CUserBaseInfo: virtual public IUserBaseInfo
{

public:
	CUserBaseInfo();

	// reimplemented (iser::IUserBaseInfo)
	virtual const imtlic::IFeatureInfoProvider* GetPermissionProvider() const override;
	virtual const imtauth::IRoleInfoProvider* GetRoleProvider() const override;
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QString GetName() const override;
	virtual void SetName(const QString& name) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual FeatureIds GetPermissions(const QByteArray& productId = QByteArray()) const override;
	virtual FeatureIds GetLocalPermissions(const QByteArray& productId) const override;
	virtual void SetLocalPermissions(const QByteArray& productId, const FeatureIds &permissions) override;
	virtual FeatureIds GetProhibitions() const override;
	virtual void SetProhibitions(const FeatureIds &prohibitions) override;
	virtual QByteArrayList GetProducts() const override;
	virtual bool RemoveProduct(const QByteArray& productId) override;
	virtual RoleIds GetRoles(const QByteArray& productId) const override;
	virtual void SetRoles(const QByteArray& productId, const RoleIds &roles) override;
	virtual void AddRole(const QByteArray& productId, const QByteArray& roleId) override;
	virtual bool RemoveRole(const QByteArray& productId, const QByteArray& roleId) override;
	virtual const imtauth::IUserGroupInfoProvider* GetUserGroupProvider() const override;

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
	const imtauth::IUserGroupInfoProvider* m_userGroupInfoProviderPtr;

protected:
//	IUserBaseInfo::FeatureIds m_permissions;
	IUserBaseInfo::FeatureIds m_restrictions;

	QMap<QByteArray, QByteArrayList> m_permissionsMap;
	QMap<QByteArray, QByteArrayList> m_rolesMap;
	QString m_name;
	QByteArray m_id;
	QString m_description;
};


} // namespace imtauth



