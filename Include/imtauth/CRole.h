#pragma once


// ImtCore includes
#include <imtauth/IRole.h>


namespace imtauth
{

class CRole: virtual public IRole
{

public:
	CRole();

	// reimplemented (iser::IRole)
	virtual const imtlic::IFeatureInfoProvider* GetPermissionProvider() const override;
	virtual QByteArray GetRoleId() const override;
	virtual void SetRoleId(const QByteArray &id) override;
	virtual QString GetRoleName() const override;
	virtual void SetRoleName(const QString &name) override;
	virtual IRole::FeatureIds GetPermissions() const override;
	virtual IRole::FeatureIds GetLocalPermissions() const override;
	virtual void SetLocalPermissions(const IRole::FeatureIds &permissions) override;
	virtual IRole::FeatureIds GetProhibitions() const override;
	virtual void SetProhibitions(const IRole::FeatureIds &prohibitions) override;
	virtual const QList<const IRole*> GetParents() const override;
	virtual void SetParents(const QList<const IRole*> parents) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	const imtlic::IFeatureInfoProvider* m_permissionProviderPtr;

private:
    QByteArray m_roleId;
    QString m_roleName;
	IRole::FeatureIds m_rolePermissions;
	IRole::FeatureIds m_roleRestrictions;
	QList<const IRole*> m_parents;
};


} // namespace imtauth



