// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>

// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IEnableableParam.h>
#include <iprm/ITextParam.h>
#include <iser/IVersionInfo.h>

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
	The base DN (e.g. DC=example,DC=com) limits the directory query. The name filter is applied locally:
	groups that do not match are not newly mirrored, while existing mirrors are retained; groups outside the base-DN subtree are treated as removed.
	If the internal user collection is connected, the direct members (users) of the LDAP groups are synchronized too:
	a user already known in the system (same SID, or same login if the stored user has no SID) is only added to the mirrored groups,
	an unknown user is added to the user collection as LDAP user and then to the mirrored groups.
	Users of the LDAP system and the users found in the directory are removed from a mirrored group if they are no longer its members,
	other members added by an administrator are kept.
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
			Distinguished name of the group, used to resolve the group memberships of the users.
		*/
		QString dn;
		/**
			\c false for a directory group outside of the name filter: it is not mirrored, but an existing mirror is kept.
		*/
		bool isMatching = true;
	};

	typedef QList<LdapGroup> LdapGroups;

	/**
		Enabled directory user being a direct member of at least one of the read LDAP groups.
	*/
	struct LdapUser
	{
		QByteArray sid;
		/**
			Login of the user: the user principal name (user@domain) if available, otherwise the account name.
		*/
		QByteArray login;
		/**
			Account name (sAMAccountName) of the user.
		*/
		QByteArray accountName;
		/**
			Down-level logon name (DOMAIN\account name) of the user, empty if the domain name could not be resolved.
		*/
		QByteArray domainAccountName;
		QString name;
		QString description;
		QString mail;
		/**
			SIDs of the LDAP groups the user is a direct member of.
		*/
		QByteArrayList groupSids;
	};

	typedef QList<LdapUser> LdapUsers;

	/**
		Item of the group tree, see CreateGroupTree.
	*/
	struct LdapGroupTreeItem
	{
		/**
			Index of the group in the LDAP group list.
		*/
		int groupIndex = -1;
		/**
			Depth in the tree, 0 for a root group.
		*/
		int level = 0;
	};

	typedef QList<LdapGroupTreeItem> LdapGroupTree;

	/**
		Progress callback of ReadLdapGroups, called with the number of groups read so far.
		Return \c false to cancel the reading.
	*/
	typedef std::function<bool (int readGroupCount)> ReadingProgressFunction;

	I_BEGIN_COMPONENT(CLdapUserGroupSynchronizerComp);
		I_ASSIGN(m_userGroupCollectionCompPtr, "UserGroupCollection", "Internal user group collection", true, "UserGroupCollection");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Internal user collection, if connected the members of the LDAP groups are synchronized too", false, "UserCollection");
		I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "The interval for synchronization LDAP groups (in secs), 0 disables the synchronization", false, 60);
		I_ASSIGN(m_ldapSystemIdAttrPtr, "LdapSystemId", "LDAP System-ID", false, "Ldap");
		I_ASSIGN(m_ldapSystemNameAttrPtr, "LdapSystemName", "LDAP System Name", false, "LDAP");
		I_ASSIGN(m_groupNameFilterAttrPtr, "GroupNameFilter", "Name filter of the synchronized LDAP groups, '*' is a wildcard", false, "*");
		I_ASSIGN(m_groupNameFilterParamCompPtr, "GroupNameFilterParam", "Optional name filter parameter, overrides GroupNameFilter", false, "GroupNameFilterParam");
		I_ASSIGN(m_baseDnParamCompPtr, "BaseDnParam", "Distinguished name of the directory subtree containing the synchronized LDAP groups (e.g. DC=example,DC=com), the domain of the computer is used if empty", false, "BaseDnParam");
		I_ASSIGN(m_transactionManagerCompPtr, "TransactionManager", "Transaction manager of the user group collection, all changes of a synchronization are committed or rolled back together", false, "TransactionManager");
		I_ASSIGN_MULTI_0(m_enableableParamsCompPtr, "EnableableParams", "The groups are synchronized only while all these parameters are enabled", false);
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info used to store the user groups, the groups are not synchronized if it cannot store the system flag of a group", false, "VersionInfo");
	I_END_COMPONENT;

	CLdapUserGroupSynchronizerComp();

	/**
		Get the ID of the internal group mirroring the LDAP group with the given SID.
	*/
	static QByteArray GetGroupObjectId(const QByteArray& sid);

	/**
		Get the ID of the internal user created for the LDAP user with the given SID.
	*/
	static QByteArray GetUserObjectId(const QByteArray& sid);

	/**
		Check if the group name matches the name filter, '*' is a wildcard and the comparison is case-insensitive.
	*/
	static bool IsMatchingName(const QString& nameFilter, const QString& name);

	/**
		Get the ADSI path of the directory subtree searched for the groups.
		An empty base DN results in an empty path: the default naming context (domain of the computer) is used.
		A distinguished name (e.g. DC=example,DC=com) is converted to an LDAP path, a given LDAP path is used unchanged (the provider name is normalized to LDAP://).
	*/
	static QString GetSearchPath(const QString& baseDn);

	/**
		Get the LDAP search filter of the groups matching the name filter.
		'*' is kept as wildcard, the other special characters are escaped (RFC 4515). An empty filter or '*' selects all groups.
	*/
	static QString GetSearchFilter(const QString& nameFilter);

	/**
		Get the ADSI path of the domain containing the base DN, the users are searched in the whole domain.
		The path is the search path (see GetSearchPath) reduced to the domain components (DC=...) of the DN.
		An empty base DN results in an empty path (default naming context), a path without domain components is used unchanged.
	*/
	static QString GetDomainSearchPath(const QString& baseDn);

	/**
		Get the LDAP search filters of the users being direct members (memberOf) of the given groups.
		The distinguished names are escaped (RFC 4515), duplicates (case-insensitive) and empty names are ignored.
		Each filter contains at most \c maxGroupCount groups, no filter is returned for an empty group list.
	*/
	static QStringList GetUserSearchFilters(const QStringList& groupDns, int maxGroupCount = 100);

	/**
		Read all LDAP groups below the base DN, the groups not matching the name filter are marked (see LdapGroup::isMatching).
		The name filter is not part of the directory query: existing mirrors of groups no longer matching the filter and the nesting to them must remain in the snapshot.
		Returns \c false if the directory or any of the groups could not be read completely or the reading was cancelled.
		The optional progress function is called before the search and after each read group, the reading is cancelled if it returns \c false.
		It is called in the reading thread.
	*/
	static bool ReadLdapGroups(
				const QString& nameFilter,
				const QString& baseDn,
				LdapGroups& groups,
				const ReadingProgressFunction& progressFunction = ReadingProgressFunction());

	/**
		Create the tree of the groups matching the name filter in depth-first order, the groups of a level are sorted by name.
		Parent groups not matching the name filter are ignored: a group without a matching parent is a root group.
		A group with several parent groups appears below each parent, cyclic memberships are shown once per branch.
	*/
	static LdapGroupTree CreateGroupTree(const LdapGroups& groups);

	/**
		Read the enabled users of the domain of the base DN being direct members of the given groups (see LdapGroup::dn).
		Only the members of the groups are queried (see GetUserSearchFilters), users without membership in any of the groups are not returned.
		Returns \c false if the directory or any of the users could not be read completely.
	*/
	static bool ReadLdapUsers(const QString& baseDn, const LdapGroups& groups, LdapUsers& users);

	/**
		Get the LDAP groups mirrored by the synchronization: the groups matching the name filter and the groups with an existing mirror in the user group collection.
		Only the members of these groups must be read (see ReadLdapUsers).
		Returns \c false if the user group collection is not connected or could not be read.
	*/
	bool GetMirroredLdapGroups(const LdapGroups& ldapGroups, LdapGroups& mirroredGroups) const;

	/**
		Apply the given complete snapshot of the LDAP groups to the internal group collection.
		If a snapshot of the group members is given, the members of the mirrored groups are synchronized too (the user collection must be connected),
		otherwise the members of the groups are not changed.
		Nothing is changed if the snapshot or the stored groups or users are inconsistent or could not be read.
	*/
	bool SynchronizeGroups(const LdapGroups& ldapGroups, const LdapUsers* ldapUsersPtr = nullptr);

	/**
		Remove the given groups from the membership lists of all current users of the user collection, also of the users not managed by LDAP.
		Returns \c false if the users could not be read or a changed user could not be written.
	*/
	bool RemoveGroupMemberships(imtbase::IObjectCollection& userCollection, const imtbase::ICollectionInfo::Ids& groupIds) const;

	/**
		Get the configured base DN of the directory search, empty if not configured.
	*/
	QString GetBaseDn() const;

	/**
		Get the current name filter, falling back to GroupNameFilter if no parameter is connected.
	*/
	QString GetGroupNameFilter() const;

	/**
		Check if all connected enableable parameters are enabled.
	*/
	bool IsSynchronizationEnabled() const;

	/**
		Check if the connected version info stores the system flag of the groups.
		Without a connected version info the version is not checked.
	*/
	bool IsSystemFlagStored() const;

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
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_ATTR(int, m_checkIntervalAttrPtr);
	I_ATTR(QByteArray, m_ldapSystemIdAttrPtr);
	I_TEXTATTR(m_ldapSystemNameAttrPtr);
	I_TEXTATTR(m_groupNameFilterAttrPtr);
	I_REF(iprm::ITextParam, m_groupNameFilterParamCompPtr);
	I_REF(iprm::ITextParam, m_baseDnParamCompPtr);
	I_REF(imtbase::ITransactionManager, m_transactionManagerCompPtr);
	I_MULTIREF(iprm::IEnableableParam, m_enableableParamsCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
};


} // namespace imtauth

