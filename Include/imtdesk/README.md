# imtdesk Module

## Overview

The **imtdesk** module provides the core domain model for the Service Desk
ticketing system. It defines interfaces and ACF component implementations for
support tickets with type, priority, status, and environment fields.

## Key Features

- **Tickets**: Service Desk tickets with type, priority, status, and environment fields
- **Ticket Lifecycle**: Open → InProgress → Resolved → Closed
- **Service Interface**: `ITicketService` for ticket CRUD operations
- **Integration**: Tickets can be linked to conversations and messages from imtchat

## Architecture

### Interfaces

| Interface         | Purpose                                        |
|-------------------|------------------------------------------------|
| `ISupportTicket`  | Ticket data (title, type, status, priority)    |
| `ITicketService`  | Ticket CRUD operations                         |

### ACF Components

| Component             | Implements       |
|-----------------------|------------------|
| `CSupportTicketComp`  | `ISupportTicket` |

### Design Patterns

- **Interface Segregation**: Clean separation between chat and ticketing domains
- **Component Pattern**: ACF-based components with `I_BEGIN_COMPONENT` / `I_END_COMPONENT`
- **Immutable IDs**: IDs use `QByteArray`; display strings use `QString`

## Related Modules

- **imtdeskdb** — PostgreSQL/SQLite persistence layer
- **imtdeskgql** — GraphQL API binding
- **imtdeskgui** — QML user interface
- **imtchat** — Chat domain model
