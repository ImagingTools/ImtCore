// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IPartyRoleCatalog.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	The system roles of the .acc file and the roles the organizations keep in a collection.
*/
class CPartyRoleCatalogComp:
			public ilog::CLoggerComponentBase,
			virtual public IPartyRoleCatalog
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPartyRoleCatalogComp);
		I_REGISTER_INTERFACE(IPartyRoleCatalog);
		I_ASSIGN_MULTI_0(m_systemRolesCompPtr, "SystemRoles", "Roles available to every organization", false);
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Roles of the organizations", true, "PartyRoleCollection");
		I_ASSIGN(m_roleFactoryCompPtr, "RoleFactory", "Factory of a role", true, "PartyRoleInfo");
	I_END_COMPONENT;

	// reimplemented (imtauth::IPartyRoleCatalog)
	virtual PartyRoleInfoList GetRoles(const QByteArray& tenantId, bool includeArchived, bool* isReadPtr = nullptr) const override;
	virtual IPartyRoleInfoUniquePtr GetRole(const QByteArray& roleId) const override;
	virtual QByteArray CreateRole(const IPartyRoleInfo& role, QString& errorMessage) override;
	virtual bool UpdateRole(const IPartyRoleInfo& role, QString& errorMessage) override;
	virtual bool SetRoleArchived(const QByteArray& tenantId, const QByteArray& roleId, bool isArchived, QString& errorMessage) override;

private:
	IPartyRoleInfoUniquePtr CopyRole(const IPartyRoleInfo& role) const;
	IPartyRoleInfoUniquePtr ReadStoredRole(const QByteArray& roleId) const;
	bool StoreRole(const IPartyRoleInfo& role, bool isNew);
	bool CheckRole(const IPartyRoleInfo& role, QString& errorMessage) const;

private:
	I_MULTIREF(IPartyRoleInfo, m_systemRolesCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_FACT(IPartyRoleInfo, m_roleFactoryCompPtr);

	// The check of a name and the change are one step
	QMutex m_changeMutex;
};


} // namespace imtauth


