# imtchat Module

## Overview

The **imtchat** module provides the core domain model for the internal Service Desk
chat system. It defines interfaces and ACF component implementations for conversations,
messages, and entity references.

## Key Features

- **Conversations**: Direct (1:1), Group, Channel, and Support conversation types
- **Messages**: Rich text messages with entity references and attachment IDs
- **Entity References**: Link any ImtCore domain object inside a message body
- **Service Interfaces**: `IChatService` for business-logic consumers

## Architecture

### Interfaces

| Interface         | Purpose                                        |
|-------------------|------------------------------------------------|
| `IConversation`   | Conversation data (id, name, type, participants) |
| `IChatMessage`        | Message data (id, content, sender, status)     |
| `IEntityReference`| Reference to an external ImtCore entity       |
| `IChatService`    | Chat operations (send, get, create, mark-read) |

### ACF Components

| Component             | Implements      |
|-----------------------|-----------------|
| `CConversationComp`   | `IConversation` |
| `CChatMessageComp`    | `IChatMessage`  |

### Design Patterns

- **Interface Segregation**: Separate interfaces for conversations and messages
- **Component Pattern**: ACF-based components with `I_BEGIN_COMPONENT` / `I_END_COMPONENT`
- **Immutable IDs**: IDs use `QByteArray`; display strings use `QString`

## Related Modules

- **imtchatdb** — PostgreSQL/SQLite persistence layer
- **imtchatgql** — GraphQL API binding
- **imtchatgui** — QML user interface
- **imtdesk** — Service Desk ticketing domain model
