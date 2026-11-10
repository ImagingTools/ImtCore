# LDAP Group Synchronization

User groups carry a system flag like users do (`IUserGroupInfo::GetSystemInfos`):

| System | `systemId` | Created by |
|---|---|---|
| Internal | empty | administrators, through the group editor |
| LDAP | `Ldap` (configurable) | `LdapUserGroupSynchronizer` |

The system of a group cannot be changed through GraphQL. `GroupItem` returns it in `systemInfos`. `GroupsList` returns it in `systemId` and `systemName`.

## Synchronization

`LdapUserGroupSynchronizer` (`imtauth::CLdapUserGroupSynchronizerComp`) mirrors the LDAP group hierarchy into the internal group collection:

- each LDAP group matching `GroupNameFilter` becomes an internal group with the LDAP system flag. Its ID is derived from the group SID, so the ID stays the same when the group is renamed;
- LDAP nesting (`memberOf`) becomes the parent groups. Only groups covered by the filter can be parents;
- the name and description follow the directory;
- the assignments an administrator makes to a mirrored group remain: roles, users and internal parent groups;
- mirrored groups that disappear from the directory are removed. Internal groups are never removed.

The directory is read every `CheckInterval` seconds (default 60; `0` disables the background synchronization). If the directory or a stored group cannot be read, the cycle changes nothing.

Reading the directory is supported on Windows (ADSI, domain of the server account) only. On other platforms the component does nothing.

## Configuration

| Name | Type | Default | Description |
|---|---|---|---|
| `UserGroupCollection` | reference | — | The internal user group collection (required). |
| `CheckInterval` | int | 60 | Synchronization interval in seconds. |
| `LdapSystemId` | string | `Ldap` | System ID of the mirrored groups. |
| `LdapSystemName` | string | `LDAP` | System name of the mirrored groups. |
| `GroupNameFilter` | string | `*` | LDAP `name` filter; `*` is the only wildcard. |

## Data format

`CUserGroupInfo` serializes `SystemInfos` starting with ImtCore version 23950. Groups stored by older versions are read as internal groups.
