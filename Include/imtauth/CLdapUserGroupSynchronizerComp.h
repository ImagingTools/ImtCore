// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	Synchronizes the LDAP group hierarchy with the internal user group hierarchy.
	Every LDAP group is mirrored as an internal group belonging to the LDAP system,
	nested LDAP groups become parent groups. Internal groups and assignments made to
	the mirrored groups (e.g. roles) are kept.
*/
class CLdapUserGroupSynchronizerComp: public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	struct LdapGroup
	{
		QByteArray sid;
		QString name;
		QString description;
		QByteArrayList parentSids;
	};

	typedef QList<LdapGroup> LdapGroups;

	I_BEGIN_COMPONENT(CLdapUserGroupSynchronizerComp);
		I_ASSIGN(m_userGroupCollectionCompPtr, "UserGroupCollection", "Internal user group collection", true, "UserGroupCollection");
		I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "The interval for synchronization LDAP groups (in secs), 0 disables the synchronization", false, 60);
		I_ASSIGN(m_ldapSystemIdAttrPtr, "LdapSystemId", "LDAP System-ID", false, "Ldap");
		I_ASSIGN(m_ldapSystemNameAttrPtr, "LdapSystemName", "LDAP System Name", false, "LDAP");
		I_ASSIGN(m_groupNameFilterAttrPtr, "GroupNameFilter", "Name filter of the synchronized LDAP groups, '*' is a wildcard", false, "*");
	I_END_COMPONENT;

	CLdapUserGroupSynchronizerComp();

	/**
		Get the ID of the internal group mirroring the LDAP group with the given SID.
	*/
	static QByteArray GetGroupObjectId(const QByteArray& sid);

	/**
		Read the LDAP groups matching the name filter. Returns \c false if the directory could not be read.
	*/
	static bool ReadLdapGroups(const QString& nameFilter, LdapGroups& groups);

	/**
		Apply the given LDAP groups to the internal group collection.
	*/
	bool SynchronizeGroups(const LdapGroups& ldapGroups);

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	class SynchronizationThread: public QThread
	{
	public:
		explicit SynchronizationThread(CLdapUserGroupSynchronizerComp& parent);

	protected:
		// reimplemented (QThread)
		virtual void run() override;

	private:
		CLdapUserGroupSynchronizerComp& m_parent;
	};

protected:
	SynchronizationThread m_synchronizationThread;

protected:
	I_REF(imtbase::IObjectCollection, m_userGroupCollectionCompPtr);
	I_ATTR(int, m_checkIntervalAttrPtr);
	I_ATTR(QByteArray, m_ldapSystemIdAttrPtr);
	I_TEXTATTR(m_ldapSystemNameAttrPtr);
	I_TEXTATTR(m_groupNameFilterAttrPtr);
};


} // namespace imtauth


