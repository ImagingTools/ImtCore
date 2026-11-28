# User administration in ImtCore products

This document is part of the knowledge base of the AI assistant. It summarizes the concepts of the
user administration shared by all ImtCore products (Puma, ProLife, Agentino, Dogma, Claire, ...) and
how tasks map to the public GraphQL API. Details: `Permissions-Rules.en.md` and the security documents.

## Concepts

- **Organization (tenant):** a company or team. Users, roles and groups belong to an organization. Inside
  an organization only its own users, roles and groups are visible and count. A user works in one
  organization at a time (the organization of the session).
- **User:** an account with a login (`username`), a name and an e-mail address. A user gets permissions
  through roles, groups and directly assigned permissions.
- **Role:** a named set of permissions of one product, e.g. "Operator". A role can have parent roles
  whose permissions it includes.
- **Group:** a set of users with roles. Every member gets the roles of the group. Groups can have parent
  groups. Groups can come from LDAP (synchronized, read-only).
- **Permission:** a path in the permission tree of a product, e.g. `/Administration/Users/View`.
  A permission can include other permissions (requirements), e.g. `Edit/Change` includes `View`.
- **Permission scope of an organization:** the upper bound of the permissions inside it. A role of the
  organization may contain only permissions of the scope. An empty scope grants nothing.
- **Owner of an organization:** holds the whole scope of the organization, nothing beyond it.
- **Superuser (`su`):** the system administrator; bypasses permission checks.
- **Product:** roles and permissions belong to a product (`productId`). Roles of another product do not
  grant anything in this one.
- **Revisions:** users, roles and groups keep their history; restoring needs separate permissions.

## Rules

- The server is the security boundary: every command checks the permissions of the user and the
  organization. What a user cannot do in the console, the assistant cannot do either.
- Group names and role IDs are unique inside an organization.
- Role and group cycles are refused (a group cannot be its own parent, directly or indirectly).
- A user who is added to a group gets the roles and permissions of the group at once.
- Passwords, tokens and sessions are never handled by the assistant.

## Common tasks and API commands

| Task | Commands |
|---|---|
| Find users | `UsersList` (items: `id`, `name`, `userId` = login, `groups`, `roles` as `;`-separated IDs) |
| Read a user with roles and groups | `UserItem` (`input: {id, productId}`) |
| Find groups | `GroupsList` (items: `id`, `name`, `description`, `users`, `roles`, `usersCount`) |
| Read a group | `GroupItem` (`input: {id, productId}`): `users`, `roles`, `parentGroups` as assignments |
| Create a group | `GroupAdd` |
| Change a group: members, roles, parents, name | `GroupUpdate` |
| Find roles | `RolesList` (items: `id`, `roleId`, `roleName`, `roleDescription`) |
| Read a role with permissions | `RoleItem` |

**Assignments** (`users`, `roles`, `parentGroups`, `permissions` of an item) are objects
`{id, name, direct, sources}`. `direct: true` means assigned to this object; other assignments are
inherited (e.g. through a parent group) and only shown.

**Update commands replace the whole object.** `GroupUpdate` and `RoleUpdate` store exactly the data they
get. To change one thing:
1. read the object with its item command (`GroupItem`, `RoleItem`);
2. send all its values back: `id`, `typeId` ("Group" or "Role"), `productId`, `name`, `description`, and in
   `item` the same plus all **direct** assignments as `{id, direct: true}`, with the change applied;
3. never drop direct assignments that should stay: a missing member is removed from the group.

Example: add the user `u7` to the group `g1` that has the members `u1`, `u2` and the role `r1`:

```json
{"id": "g1", "typeId": "Group", "productId": "<product>", "name": "Support", "description": "...",
 "item": {"id": "g1", "productId": "<product>", "name": "Support", "description": "...",
          "users": [{"id": "u1", "direct": true}, {"id": "u2", "direct": true}, {"id": "u7", "direct": true}],
          "roles": [{"id": "r1", "direct": true}], "parentGroups": []}}
```

The `productId` of the session is the product of the server; take it from the items that were read.

## Working with the assistant

- The assistant reads data with queries and proposes changes as a plan. The user sees every step with
  the data it sends and confirms or rejects the whole plan. Steps run in order and stop at the first
  failure; applied steps are not undone automatically, but they can be rolled back afterwards (below).
- For several similar changes (e.g. add ten users to a group) one update with all members is better than
  ten steps.
- The assistant never invents IDs: it finds them with list and item queries first.

## Undoing a confirmed change

Every confirmed change of the assistant is a document action: the result of the change names its ID
("Document action ...").

- `changes.list` lists the latest document actions of the user, newest first, with `reversible` and, if not,
  the `reason`. Use it when the user wants to undo something and does not name the document action.
- `changes.rollback` with the `documentActionId` rolls the document action back as a whole after the user confirmed
  the preview: changed documents get their state before the document action, documents it created are removed.
  The rollback is a document action of its own.

What can be rolled back: changes of user groups (create, members, roles, parent groups, name) in Puma.
Not rolled back: document actions that removed documents, that changed data without revisions (e.g. the
organization or its members), or documents that were changed again after the document action — then tell the
user which documents are in the way; a newer document action may have to be rolled back first.
Memberships mirrored in the user documents follow the group, as when the group is edited.
