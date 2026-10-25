# Server Change Checklist

Use this checklist for every change to server-side code: managers, GraphQL controllers, database delegates, and the components that connect them.
Go through it twice:
- before writing code, as design questions;
- before asking for review, as a self-check.

Each item comes from a defect that a review found after the code was "done".

## 1. Model the states before writing a rule

Before you write a rule over an entity's lifecycle, list every state and transition the rule depends on:

- several entities of the same kind at once, for example two relationships of the same pair of organizations;
- archived versus temporarily invalid, for example outside a validity window;
- removal followed by re-creation;
- unknown or missing values, for example a timestamp that was never set.

A rule that only fits the path you are looking at fails on the others.
*Example:* "a share is stale if it is older than the relationship" deleted live shares once one relationship replaced another. It also deleted the shares of a relationship whose `ValidUntil` had lapsed only temporarily.

## 2. Cover every path that causes the change

When a change must publish an event, revoke something, or update a cache, search for **every writer of that state**, not only the method you are editing. For example, grep for `SetStatus(...ARCHIVED)`.

Put the change and its side effect into one helper, so that a new path cannot forget the side effect.
*Example:* removing a relationship directly published `RelationshipRemoved`; archiving it through an approved delete proposal did not.

## 3. Check every caller before narrowing an API

Before you restrict who may call a command, or change what it returns, find its callers in **all** repositories: ImtCore, Puma (including `AuthClientSdk`), ProLife, Lisa, and the QML/web clients.

Gate the command by the permission that its sibling commands already use, not by an ad-hoc rule.
*Example:* limiting `GetUserObjectId` to "self or superuser" broke user managers in the client SDK. The correct gate is `/Administration/Users/View`, the permission that `UserItem` requires.

## 4. Check the meaning of a filter in every database

A `ComplexFilter` must select the same rows on PostgreSQL, SQLite and DuckDB.

| Behavior | Consequence |
|---|---|
| SQLite `LIKE` ignores case for ASCII only. | `FO_CONTAINS` does not match "Ромашка" against "ромашка" on SQLite. |
| PostgreSQL `ILIKE` treats `\` as an escape character. | A value that contains `\` does not match itself. |
| The filter converter does not escape `%` and `_`. | These characters act as wildcards in a value. |
| A non-UUID value in a UUID column is an SQL error on PostgreSQL. | Use `imtauth::CollectionSelection::SelectIds`, which returns no rows for such a value. |
| A collection that does not know a filter returns every id. | Always verify the loaded objects as well. |

**Known limitation:** the duplicate-name check of organizations (`ITenantManager::FindTenantIdsByName`) does not find duplicates on SQLite that differ only in the case of non-ASCII letters.

## 5. When you add a helper, use it everywhere

If you add a helper, such as a selection by columns, an access check or an event publisher, move **all** places in your change to it in the same change. Leave no hand-built copies behind.

## 6. Keep object lifetimes and lookups honest

- Never keep a raw pointer into a `DataPtr` that goes out of scope. Copy the data or keep the `DataPtr`.
- Do not add "fallback" lookups that query the same key as the direct lookup. Such a fallback is dead code that hides real errors.
- A full scan of a collection (`GetElementIds()` without a filter) is not acceptable in a request path. Select by indexed columns.

## 7. Write tests that can fail

- A fake collection or provider must be **at least as strict as the strictest real backend**. A fake that is more forgiving than SQLite hides SQLite bugs.
- Assert only on fields that exist in the SDL schema. An assertion on a field that is never returned, for example `id` instead of `objectId`, always passes.
- Mutation-check every new test: break the code it protects, see the test fail, and restore the code.
- Cover the paths from item 2 and the states from item 1, not only the happy path.

## 8. Before you say "done"

- Run a strict code review of your own diff and verify each finding against the code.
- Build and run the QtTests of every affected repository, including product tests that are not built by default (for example, Puma `pumatest` requires `BUILD_TESTING`).
- Run the API and WebSocket suites of Puma, ProLife and Lisa that touch the changed components.
- In the pull request, list the open issues and known limitations explicitly.
