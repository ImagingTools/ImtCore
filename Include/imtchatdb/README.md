# imtchatdb Module

## Overview

The **imtchatdb** module provides SQL database persistence for the imtchat domain
model. It implements database delegate components for conversations, messages,
message status, participants, and tickets using PostgreSQL or SQLite.

## Key Features

- **Dual Database Support**: PostgreSQL (production) and SQLite (embedded/offline)
- **Hard Deletes**: Conversations, Messages and Tickets use actual `DELETE FROM`
- **Upsert Patterns**: Message status uses conflict-safe upsert queries
- **Full-Text Search**: PostgreSQL Messages table includes GIN index on `Content`
- **JSONB Storage**: Entity references, attachment IDs and tags use JSONB (Postgres) / TEXT (SQLite)
- **Auto-table Creation**: Each delegate creates its table in `OnComponentCreated` if absent

## Architecture

### Database Delegates

| Component                       | Table                    | Notes                              |
|---------------------------------|--------------------------|------------------------------------|
| `CConversationDbDelegateComp`   | `Conversations`          | Hard delete                        |
| `CMessageDbDelegateComp`        | `Messages`               | GIN FTS index (PostgreSQL)         |
| `CMessageStatusDbDelegateComp`  | `MessageStatus`          | Upsert per user                    |
| `CParticipantDbDelegateComp`    | `ConversationParticipants` | Add/remove participant helpers   |
| `CTicketDbDelegateComp`         | `Tickets`                | Full CRUD with rename/description  |

All delegates inherit from `imtdb::CSqlDatabaseObjectDelegateCompBase`.

### SQL Resources

SQL creation scripts are embedded as Qt resources via `imtchatdb.qrc`:

```
:/SQL/Postgres/CreateConversationsTable.sql
:/SQL/Postgres/CreateMessagesTable.sql
:/SQL/Postgres/CreateMessageStatusTable.sql
:/SQL/Postgres/CreateParticipantsTable.sql
:/SQL/Postgres/CreateTicketsTable.sql
:/SQL/SQLite/CreateConversationsTable.sql
...
```

The helper `GetSqlResourcePath()` selects the correct prefix based on the database
driver (`QSQLITE` → `:/SQL/SQLite/`, anything else → `:/SQL/Postgres/`).

## Related Modules

- **imtchat** — domain model interfaces
- **imtchatgql** — GraphQL API layer
- **imtchatgui** — QML user interface
