# imtdeskdb Module

## Overview

The **imtdeskdb** module provides SQL database persistence for the imtdesk
domain model. It implements database delegate components that handle
ticket storage in PostgreSQL or SQLite databases.

## Database Delegates

| Component                          | Table    |
|------------------------------------|----------|
| `CSupportTicketDbDelegateComp`     | Tickets  |

## Supported Databases

- **PostgreSQL** — primary production database; uses UUID and JSONB types
- **SQLite** — embedded / offline database; uses TEXT for IDs and JSON

## Related Modules

- **imtdesk** — Service Desk domain model
- **imtdeskgql** — GraphQL API binding
- **imtdeskgui** — QML user interface
