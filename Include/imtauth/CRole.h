#pragma once


// ImtCore includes
#include <imtauth/IRole.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtlic/CProductLicensingInfo.h>


namespace imtauth
{

class CRole: virtual public IRole
{

public:
	CRole();

	void SetParentRoleProvider(const imtauth::IRoleInfoProvider* parentRoleProvider);

	// reimplemented (iser::IRole)
	virtual const IRoleInfoProvider* GetParentRolesProvider() const override;
	virtual const imtlic::IFeatureInfoProvider* GetPermissionProvider() const override;
	virtual QByteArray GetRoleId() const override;
	virtual void SetRoleId(const QByteArray &id) override;
	virtual QString GetRoleName() const override;
	virtual void SetRoleName(const QString &name) override;
	virtual QString GetRoleDescription() const override;
	virtual void SetRoleDescription(const QString &description) override;
	virtual IRole::FeatureIds GetPermissions() const override;
	virtual IRole::FeatureIds GetLocalPermissions() const override;
	virtual void SetLocalPermissions(const IRole::FeatureIds &permissions) override;
	virtual IRole::FeatureIds GetProhibitions() const override;
	virtual void SetProhibitions(const IRole::FeatureIds &prohibitions) override;
	virtual QByteArrayList GetIncludedRoles() const override;
	virtual QByteArray GetProductId() const override;
	virtual void SetProductId(const QByteArray& productId) override;
	virtual bool IncludeRole(const QByteArray& roleId) override;
	virtual void ExcludeRole(const QByteArray& roleId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArrayList GetParentRoles(const QByteArray& roleId) const;
	void GetParentRoleList(const IRole& role, QByteArrayList& roleList) const;

protected:
	const imtlic::IFeatureInfoProvider* m_permissionProviderPtr;

private:
	QByteArray m_roleId;
	QByteArray m_productId;
	QString m_roleName;
	QString m_roleDescription;
	IRole::FeatureIds m_rolePermissions;
	IRole::FeatureIds m_roleRestrictions;
	QByteArrayList m_parents;

	const imtauth::IRoleInfoProvider* m_roleInfoProviderPtr;
};


} // namespace imtauth



