#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtlic/IFeatureInfoProvider.h>


namespace imtauth
{


/**
	Interface describing a role.
*/
class IRole: virtual public iser::IObject
{
public:
	typedef QSet<QByteArray> FeatureIds;

	/**
		Get list of all availiable permissions for this role.
	*/
	virtual const imtlic::IFeatureInfoProvider* GetPermissionProvider() const = 0;

	/**
		Get role ID.
	*/
	virtual QByteArray GetRoleId() const = 0;

	/**
		Set role ID.
	*/
	virtual void SetRoleId(const QByteArray& id) = 0;

	/**
		Get role name.
	*/
	virtual QString GetRoleName() const = 0;

	/**
		Set role name.
	*/
	virtual void SetRoleName(const QString& name) = 0;

	/**
		Get result permissions of role.
	*/
	virtual FeatureIds GetPermissions() const = 0;

	/**
		Get local permissions of role.
	*/
	virtual FeatureIds GetLocalPermissions() const = 0;

	/**
		Set permissions of role.
	*/
	virtual void SetLocalPermissions(const FeatureIds &permissions) = 0;

	/**
		Get role permissions restrictions.
	*/
	virtual FeatureIds GetProhibitions() const = 0;

	/**
		Set role permissions restrictions.
	*/
	virtual void SetProhibitions(const FeatureIds &prohibitions) = 0;

	/**
		Get parents of role.
	*/
	virtual const QList<const imtauth::IRole*> GetParents() const = 0;

	/**
		Set parents of role.
	*/
	virtual void SetParents(const QList<const imtauth::IRole*> parents) = 0;


};


} // namespace imtauth


