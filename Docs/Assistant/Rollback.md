# Rolling back applied changes: design

Status: **design, not implemented**. This document describes how a confirmed change set (an
applied `api.planChanges` plan, an administration action, later also a manual bulk operation)
can be rolled back as a whole.

## Idea

Documents are stored with revisions, and a removed document is only marked as disabled. The
data of every former state therefore stays in the database. What is missing is the link between
one confirmed change set and the revisions it produced. A small **change set journal** provides
this link. It is an index to the existing revisions, not a copy of the data, and not a log of all
transactions.

## Which collections qualify today

Checked in ImtCore and in the Puma composites (November 2026):

| Collection | Delegate | Revisions | Removal |
| --- | --- | --- | --- |
| Users | `CUserDatabaseDelegateComp` (document delegate) | yes | marked as `Disabled` |
| User groups | `CUserGroupDatabaseDelegateComp` (document delegate) | yes | marked as `Disabled` |
| Roles | `CRoleDatabaseDelegateComp` (document delegate) | yes | marked as `Disabled` |
| Tenants, memberships, connections, connection codes, invitations, entity bindings | `CSqlDatabaseObjectDelegateCompBase` subclasses | no | physical `DELETE` |
| Contracts, contract types, parties, events, directory profiles | `CSerializedObjectDbDelegateCompBase` subclasses | no | physical `DELETE` |
| Assistant configuration | `CAssistantConfigurationDbDelegateComp` | no | physical `DELETE` |

Only the collections based on `imtdb::CSqlDatabaseDocumentDelegateCompBase` can be rolled back
with the journal alone. The Puma assistant currently allows the mutations `GroupAdd` and
`GroupUpdate`, which both work on user groups, so the first stage covers everything the
assistant can change today.

A command that writes to a collection without revisions, or that has effects outside the
database (e-mail, LDAP, external systems), is **irreversible**: see "Irreversible commands".

## Change set journal

One record per confirmed change set:

- `ChangeSetId`, `TenantId`, owner (user id), time, summary text of the plan, origin
  (`assistant`, later `manual`, `import`), state (`applied`, `rolledBack`, `partiallyApplied`).

One entry per written document:

- `ChangeSetId`, collection (table name), `DocumentId`, operation (`created`, `updated`,
  `removed`), revision before, revision after.

The journal holds no document data, no parameter values and no message texts. It lives in the
database of the tenant, next to the documents it refers to.

### Who writes the journal

The assistant applies changes through the GraphQL handlers and does not see revision numbers.
Reading them before and after from outside would be open to races. Instead:

1. The action that applies a change set creates the journal record and puts the `ChangeSetId`
   into the `imtbase::IOperationContext` passed down to the collections.
2. `CSqlDatabaseDocumentDelegateCompBase` adds the journal entry in the **same query** that
   writes the revision (`PrepareInsertNewObjectQuery`, the revision number expression is
   already part of that query) and in the queries that mark documents as disabled or active.
   The entry is therefore committed or rolled back together with the revision.
3. Without a `ChangeSetId` in the operation context nothing is written; existing behaviour is
   unchanged.

This makes the mechanism generic: other write paths only have to pass a `ChangeSetId`.

## Rollback

A rollback never rewrites history. It is a new change set in the opposite direction:

| Original operation | Rollback |
| --- | --- |
| `updated` | `RestoreRevision(revision before)`, which writes a new revision with the former content |
| `created` | mark the document as disabled |
| `removed` | mark the document as active again (`CreateRestoreObjectsQuery`) |

Entries are processed in reverse order. The rollback itself gets a journal record with origin
`rollback` and a reference to the original change set, so a rollback can be traced and, if
needed, rolled back again.

### Conflicts

Before anything is written, every entry is checked:

- the current revision of the document must still be the "revision after" of the entry,
- the "revision before" must still be available (`RevisionInfo::isRevisionAvailable`); a
  revision removed with `DeleteRevision` or by a clean-up makes the entry impossible to roll back.

If any check fails, the rollback is refused as a whole and the preview lists the documents that
were changed later or whose revision is gone. Partial rollbacks are not offered in the first
stage.

### Confirmation and permissions

A rollback is a change like any other: the user sees a preview (documents, operation, current
and restored state) and confirms it. The preview is bound to user, tenant and a state token
built from the current revisions, as for all assistant changes. The user needs the edit
permissions of every affected collection, checked through the same handlers. Rolling back a
change set of another user additionally requires a dedicated permission
(`/Administration/Assistant/RollbackAny`).

## Irreversible commands

The API catalog marks a mutation as irreversible when it writes to a collection without
revisions or has external effects. Configuration: a new list `IrreversibleMutations` next to
`AllowedMutations`. For a plan that contains such a step:

- the preview shows that the plan cannot be rolled back, and which step causes it,
- the journal state is `irreversible`, and a rollback is refused.

## Retention

The journal can be limited (for example 90 days per tenant) because the data stays in the
revisions; only the grouping is lost. Revision clean-up should not remove revisions referenced
by a journal entry younger than the retention period.

## Assistant integration

- `api.planChanges` and the administration actions create the journal record when the change
  is applied; the result tells the model the `ChangeSetId`.
- New read action `changes.list`: the user's recent change sets in the current tenant (summary,
  time, state).
- New change action `changes.rollback`: preview and apply as described above.
- Possible entry point in the user interface: "Undo" next to the result of an applied plan in
  the assistant panel, and a list of change sets in the administration area.

## Later

- Revisions for the collections that do not have them yet (tenants, memberships, contracts), if
  rollback is needed there. This is a data model change of its own.
- Partial rollback of selected entries.
- Rollback of manual bulk operations and imports through the same journal.
