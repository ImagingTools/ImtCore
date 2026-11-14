# LDAP Group Synchronization

User groups carry a system flag like users do (`IUserGroupInfo::GetSystemInfos`):

| System | `systemId` | Created by |
|---|---|---|
| Internal | empty | administrators, through the group editor |
| LDAP | `Ldap` (configurable) | `LdapUserGroupSynchronizer` |

The system of a group cannot be changed through GraphQL: groups created by clients (SDL, archive, import, duplicate, group document editor) are internal, updates keep the stored system. `GroupItem` and `GetGroupRepresentation` return it in `systemInfos`. `GroupsList` returns it in `systemId` and `systemName`.

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

All changes are prepared before the first write. The groups vanished from the directory are only removed after all other writes succeeded. With a `TransactionManager` all writes are committed or rolled back together.

The synchronization and the group mutations of `CUserGroupCollectionControllerComp` are serialized, so a group edited by an administrator is not overwritten with a stale copy. Group saves through the generic document service are not serialized with the synchronization.

Reading the directory is supported on Windows (ADSI, domain of the server account) only. On other platforms the component does nothing.

## Configuration

| Name | Type | Default | Description |
|---|---|---|---|
| `UserGroupCollection` | reference | — | The internal user group collection (required). |
| `CheckInterval` | int | 60 | Synchronization interval in seconds. |
| `LdapSystemId` | string | `Ldap` | System ID of the mirrored groups. |
| `LdapSystemName` | string | `LDAP` | System name of the mirrored groups. |
| `GroupNameFilter` | string | `*` | Filter of the group `name` (case-insensitive); `*` is the only wildcard. |
| `TransactionManager` | reference | — | Optional transaction manager of the group collection (e.g. `CSqlDatabaseTransactionManagerComp`). |

## Data format

`CUserGroupInfo` serializes `SystemInfos` starting with ImtCore version 23950. Groups stored by older versions are read as internal groups.

The version is taken from the `VersionInfo` of the group repository delegate. Without a connected `VersionInfo` the system flag is not stored: the synchronization then detects mirrors without the LDAP flag and stops with an error instead of creating duplicates. Connect `VersionInfo` when the synchronization is used.

The SQL group delegate stores only the direct users of a group, the users inherited from parent groups are not written as members.
