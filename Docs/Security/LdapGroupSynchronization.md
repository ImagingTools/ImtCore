# LDAP Group Synchronization

User groups carry a system flag like users do (`IUserGroupInfo::GetSystemInfos`):

| System | `systemId` | Created by |
|---|---|---|
| Internal | empty | administrators, through the group editor |
| LDAP | `Ldap` (configurable) | `LdapUserGroupSynchronizer` |

The system of a group cannot be changed through GraphQL: groups created by clients (SDL, archive, import, duplicate, group document editor) are internal, updates keep the stored system. `UserGroupCollectionController` resets duplicates and imported groups without their own object ID to the internal system, since they are stored under a new ID; an imported group keeping its object ID keeps its system. `GroupItem` and `GetGroupRepresentation` return it in `systemInfos`. `GroupsList` returns it in `systemId` and `systemName`.

## Synchronization

`LdapUserGroupSynchronizer` (`imtauth::CLdapUserGroupSynchronizerComp`) mirrors the LDAP group hierarchy into the internal group collection:

- each LDAP group matching `GroupNameFilter` becomes an internal group with the LDAP system flag. Its ID is derived from the group SID, so the ID stays the same when the group is renamed;
- a mirrored group that leaves the filter (e.g. renamed) is kept with its assignments and still follows the directory. Only groups removed from the directory are removed;
- LDAP nesting (`memberOf`) becomes the parent groups. Only mirrored groups can be parents. A nesting that would create a cycle is omitted (with a warning), existing links are kept first;
- the name and description follow the directory. A name already used by another group or by several LDAP groups gets the SID appended, e.g. `Admins (S-1-5-21-...)`;
- the assignments an administrator makes to a mirrored group remain: roles, users and internal parent groups;
- mirrored groups that disappear from the directory are removed. Internal groups are never removed.

Changes of the hierarchy take effect immediately, also for signed-in users, like changes made by an administrator.

The directory is read every `CheckInterval` seconds (default 60; `0` disables the background synchronization). The cycle changes nothing if:

- the directory or any group (SID, name, distinguished name, description, `memberOf`) cannot be read completely;
- the group list or a stored group cannot be read;
- a stored group with the ID of a mirror is not stored as an LDAP group (see [Data format](#data-format));
- `LdapSystemId` is empty.

A failed directory read is logged as an error once per failure period (not every cycle); when the directory can be read again, an info message is logged. The stored groups stay unchanged in between and may become outdated.

All changes are prepared before the first write. The groups vanished from the directory are only removed after all other writes succeeded. With a `TransactionManager` all writes are committed or rolled back together.

The synchronization and the group mutations of `CUserGroupCollectionControllerComp` are serialized, so a group edited by an administrator is not overwritten with a stale copy. Group saves through the generic document service are not serialized with the synchronization.

Reading the directory is supported on Windows (ADSI, domain of the server account) only. On other platforms the component does nothing and, if enabled, logs the read error once.

## Configuration

| Name | Type | Default | Description |
|---|---|---|---|
| `UserGroupCollection` | reference | — | The internal user group collection (required). |
| `CheckInterval` | int | 60 | Synchronization interval in seconds. |
| `LdapSystemId` | string | `Ldap` | System ID of the mirrored groups. |
| `LdapSystemName` | string | `LDAP` | System name of the mirrored groups. |
| `GroupNameFilter` | string | `*` | Filter of the group `name` (case-insensitive); `*` is the only wildcard. |
| `TransactionManager` | reference | — | Optional transaction manager of the group collection (e.g. `CSqlDatabaseTransactionManagerComp`). |
| `EnableableParams` | reference[] | — | Optional parameters; a cycle synchronizes only while all of them are enabled. They are checked at each cycle, so a change applies to the next cycle. |
| `VersionInfo` | reference | — | Optional version info of the group repository. If it reports an ImtCore version below 23950, no cycle synchronizes and an error is logged once (see [Data format](#data-format)). |

## Data format

`CUserGroupInfo` serializes `SystemInfos` starting with ImtCore version 23950. Groups stored by older versions are read as internal groups.

The version is taken from the `VersionInfo` of the group repository delegate. Without a connected `VersionInfo` the system flag is not stored: the synchronization then detects mirrors without the LDAP flag and stops with an error instead of creating duplicates. Connect `VersionInfo` when the synchronization is used.

The SQL group delegate stores only the direct users of a group, the users inherited from parent groups are not written as members.

## Puma

`PumaServerBase` (`PumaVoce`) creates `LdapUserGroupSynchronizer` automatically:

- `UserGroupCollection` is the exported `UserGroupRepository` (PostgreSQL and SQLite servers);
- `EnableableParams` are the Puma settings **LDAP / Enable LDAP** and **LDAP / Synchronize LDAP groups**. The second one is disabled by default;
- `VersionInfo` is the Puma version info (ImtCore version from `ImtCoreVoce/VersionInfo`);
- no `TransactionManager` is connected, the writes of a cycle are not committed together.

The vendored ImtCore is at version 23625, so Puma does not synchronize groups yet even if the setting is enabled, it logs the version error instead. Do not lower the gate in `CUserGroupInfo`: ImtCore stores the group SID starting with version 23930, so a raised version number requires that format too.
