# imtchatgql Module

## Overview

The **imtchatgql** module bridges the imtchat domain model with the GraphQL API
defined in the `ImtChat.sdl` schema. It provides collection controller components
that translate between C++ domain objects and SDL-generated representation types.

## Key Features

- **SDL-Generated Bases**: Controllers inherit from generated `*ControllerCompBase` classes
- **Bidirectional Mapping**: Domain objects ↔ GraphQL representations
- **Mutation Support**: Create and Update mutations for all three entity types
- **Content Preview**: Message list items truncate content to 100 characters

## Architecture

### Collection Controllers

| Component                              | SDL Query / Mutation                          |
|----------------------------------------|-----------------------------------------------|
| `CConversationCollectionControllerComp` | ConversationsList, ConversationItem, ConversationCreate, ConversationUpdate |
| `CMessageCollectionControllerComp`     | MessagesList, MessageItem, MessageSend, MessageUpdate |
| `CTicketCollectionControllerComp`      | TicketsList, TicketItem, TicketCreate, TicketUpdate |

Each controller implements:
- `CreateRepresentationFromObject()` (list items) — iterator → list DTO
- `CreateRepresentationFromObject()` (detail item) — domain object → detail DTO
- `CreateObjectFromRepresentation()` — DTO → new domain object (Create mutation)
- `UpdateObjectFromRepresentationRequest()` — update domain object from request (Update mutation)
- `FillObjectFromRepresentation()` — shared helper for create/update

### Generated Includes

Controllers include generated headers from:
```
<GeneratedFiles/imtchatsdl/SDL/1.0/CPP/ImtChat.h>
```

This file is produced by the SDL code generator from `Sdl/imtchat/1.0/ImtChat.sdl`.

## Related Modules

- **imtchat** — domain model interfaces
- **imtchatdb** — database persistence
- **imtchatgui** — QML user interface
