// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/ITransactionManager.h>


namespace imtauth
{


/**
	Synchronizes the LDAP group hierarchy with the internal user group hierarchy.
	Every LDAP group is mirrored as an internal group belonging to the LDAP system,
	nested LDAP groups become parent groups. Internal groups and assignments made to
	the mirrored groups (e.g. roles) are kept.
	The name filter only selects the directory groups that are mirrored for the first time:
	a mirrored group is kept and follows the directory until it is removed from the directory.
*/
class CLdapUserGroupSynchronizerComp: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	struct LdapGroup
	{
		QByteArray sid;
		QString name;
		QString description;
		QByteArrayList parentSids;
		/**
			\c false for a directory group outside of the name filter: it is not mirrored, but an existing mirror is kept.
		*/
		bool isMatching = true;
	};

	typedef QList<LdapGroup> LdapGroups;

	I_BEGIN_COMPONENT(CLdapUserGroupSynchronizerComp);
		I_ASSIGN(m_userGroupCollectionCompPtr, "UserGroupCollection", "Internal user group collection", true, "UserGroupCollection");
		I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "The interval for synchronization LDAP groups (in secs), 0 disables the synchronization", false, 60);
		I_ASSIGN(m_ldapSystemIdAttrPtr, "LdapSystemId", "LDAP System-ID", false, "Ldap");
		I_ASSIGN(m_ldapSystemNameAttrPtr, "LdapSystemName", "LDAP System Name", false, "LDAP");
		I_ASSIGN(m_groupNameFilterAttrPtr, "GroupNameFilter", "Name filter of the synchronized LDAP groups, '*' is a wildcard", false, "*");
		I_ASSIGN(m_transactionManagerCompPtr, "TransactionManager", "Transaction manager of the user group collection, all changes of a synchronization are committed or rolled back together", false, "TransactionManager");
	I_END_COMPONENT;

	CLdapUserGroupSynchronizerComp();

	/**
		Get the ID of the internal group mirroring the LDAP group with the given SID.
	*/
	static QByteArray GetGroupObjectId(const QByteArray& sid);

	/**
		Check if the group name matches the name filter, '*' is a wildcard and the comparison is case-insensitive.
	*/
	static bool IsMatchingName(const QString& nameFilter, const QString& name);

	/**
		Read all LDAP groups, the groups outside of the name filter are marked as not matching.
		Returns \c false if the directory or any of the groups could not be read completely.
	*/
	static bool ReadLdapGroups(const QString& nameFilter, LdapGroups& groups);

	/**
		Apply the given complete snapshot of the LDAP groups to the internal group collection.
		Nothing is changed if the snapshot or the stored groups are inconsistent or could not be read.
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
	I_REF(imtbase::ITransactionManager, m_transactionManagerCompPtr);
};


} // namespace imtauth


