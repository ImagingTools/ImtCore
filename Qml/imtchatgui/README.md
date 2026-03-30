# imtchatgui Module

## Overview

The **imtchatgui** module provides the QML user interface for the internal Service
Desk chat system. It contains panels, dialogs, and reusable components for
conversations, messages, and tickets.

## Key Features

- **Conversation Sidebar**: `ChatPanel` with search and unread badge
- **Message View**: `ChatView` with `ListView` in `BottomToTop` order
- **Rich Message Bubbles**: `MessageBubble` shows sender, content, timestamp, delivery status
- **Smart Input**: `MessageInput` with `@` mention and `#` entity-reference autocomplete
- **Entity Reference Cards**: `EntityReferenceCard` in compact chip and full card modes
- **Ticket Management**: Create, view and list tickets; `TicketBadge` for status/priority
- **Real-time UI**: `TypingIndicator` with bounce animation; `ChatNotificationBanner` with auto-dismiss

## Component Reference

| File                         | Description                                        |
|------------------------------|----------------------------------------------------|
| `ChatPanel.qml`              | Sidebar: conversation list with search             |
| `ChatView.qml`               | Main chat area with message list and input         |
| `MessageBubble.qml`          | Single message with sender, content, status ticks  |
| `MessageInput.qml`           | Text input with entity reference support           |
| `EntityReferenceCard.qml`    | Chip (compact) or card (full) for entity refs      |
| `EntitySearchPopup.qml`      | Autocomplete popup for @ / # mentions              |
| `TicketCreateDialog.qml`     | Modal dialog for creating a support ticket         |
| `TicketView.qml`             | Ticket detail: metadata, status change actions     |
| `TicketListPanel.qml`        | Ticket list with status filter tabs                |
| `TicketBadge.qml`            | Colour-coded badge for ticket status or priority   |
| `ChatSearchBar.qml`          | Reusable search input with clear button            |
| `ConversationCreateDialog.qml` | Modal dialog for creating a new conversation    |
| `TypingIndicator.qml`        | Animated three-dot typing indicator               |
| `ChatNotificationBanner.qml` | Slide-in banner for new messages with auto-dismiss |

## Imports Used

All QML files use:
```qml
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
```

## Related Modules

- **imtchat** — domain model
- **imtchatdb** — database persistence
- **imtchatgql** — GraphQL API
