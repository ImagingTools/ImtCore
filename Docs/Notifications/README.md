# Events / Activity — User Notification System

A source-agnostic, per-user notification inbox (the **Events** page) analogous to
the *Activity* page in Microsoft Teams. Any feature — support tickets, chat,
tenant membership, licensing — can raise a notification addressed to a specific
user. The user sees a chronological, filterable feed on a dedicated **Events**
page that is always available regardless of permissions, a glowing menu
indicator while there are unread events, and a global popup when a new event
arrives in real time.

This feature **generalizes** the pre-existing per-feature ticket notifiers
(`imtdeskgql::CTicketMessageNotifierComp` / `CTicketAssigneeNotifierComp`): one
notification model, one publisher, one inbox. Adding a new producer requires
**no change** to this subsystem (Open–Closed).

---

## 1. Architecture

```
Producer (tickets, chat, tenants, …)
        │  raises
        ▼
imtnotify::INotificationService ──────────► imtbase::IObjectCollection
   (RaiseNotification / GetNotifications          (SqlDatabaseCollection)
    / filter+sort+page / MarkAllRead …)                 │
        ▲                                                ▼
        │ uses                          imtnotifydb::CNotificationDbDelegateComp
        │                                 ("Notifications" table, Postgres/SQLite,
        │                                  server-side WHERE / ORDER BY / LIMIT)
imtnotifygql::CNotificationsController      imtnotifygql::CNotificationPublisher
   (queries/mutations, scoped to the           (observes the collection, pushes
    authenticated user)                         OnNotificationReceived to the
        ▲                                        recipient only)
        │ GraphQL                                        │ WebSocket
        ▼                                                 ▼
imtnotifyguigql::GqlNotificationApiClient   (the ONLY QML that speaks GraphQL)
        │ implements
        ▼
imtnotifygui::NotificationApiClient  (abstract transport contract — no GQL)
        ▲ injected into
        │
ActivityController (singleton) ── ActivityPage / NotificationFilterBar /
        │                          NotificationDelegate / NotificationPopup
        ▼
   MenuPanel badge  ("UpdatePageBadge" → glowing indicator)
```

### SOLID notes
- **Single Responsibility** — `imtnotify` only stores/lists notifications; it has
  no knowledge of tickets, chat, etc.
- **Open–Closed** — a new producer integrates by calling `INotificationService`;
  neither the model, the publisher, nor the UI change.
- **Dependency Inversion** — producers depend only on `INotificationService`;
  the QML UI depends only on the abstract `NotificationApiClient`, never on
  GraphQL. The transport (GraphQL today) is swappable without touching the UI:
  all GraphQL lives in the separate `imtnotifyguigql` module.

---

## 2. Modules & components

| Layer | Module | Component(s) |
|-------|--------|--------------|
| Domain model | `imtnotify` (`Include/imtnotify`) | `CNotificationComp` (data), `CNotificationServiceComp` (`INotificationService`) |
| DB persistence | `imtnotifydb` (`Include/imtnotifydb`) | `CNotificationDbDelegateComp` (SQL delegate + `CreateAdditionalFiltersQuery`) |
| SDL schema | `Sdl/imtnotify/1.0/Notifications.sdl` | queries, mutations, `OnNotificationReceived` subscription |
| GraphQL | `imtnotifygql` (`Include/imtnotifygql`) | `CNotificationsControllerComp` (queries/mutations), `CNotificationPublisherComp` (live push) |
| Packages | `Impl/ImtNotifyPck`, `Impl/ImtNotifyDbPck`, `Impl/ImtNotifyGqlPck` | ACF component exports |
| QML client (no GQL) | `Qml/imtnotifygui` | `NotificationApiClient` (abstract), `ActivityController` (singleton), `ActivityPage`, `NotificationFilterBar`, `NotificationDelegate`, `NotificationPopup` |
| QML transport | `Qml/imtnotifyguigql` | `GqlNotificationApiClient` (`GqlSdlRequestSender` + SDL types) |
| Shell wiring | `Qml/imtgui` | `ApplicationMain` (inject + start/stop), `MenuPanel*` (badge) |

---

## 3. Data model & database

`imtnotify::INotification` fields: `id`, `recipientId` (drives all scoping),
`category`, `title`, `body`, `iconName`, `severity`
(`Info`/`Success`/`Warning`/`Critical`), `isRead`, `sourceType`, `sourceId`,
`targetRoute` (client deep-link), `metadata`, `createdAt`, `updatedAt`.

Persistence is an explicitly-columned **`Notifications`** table
(`Include/imtnotifydb/Resources/SQL/{Postgres,SQLite}/CreateNotificationsTable.sql`),
auto-created by the delegate base class from these scripts (`AutoCreateTable` /
`CreateTableScriptPath`).

Each SQL script contains **only the `CREATE TABLE` statement** — no indexes.
Indexes (`RecipientId`, `CreatedAt`, `(RecipientId, IsRead)`,
`(RecipientId, Category)`) are created separately by
`CNotificationDbDelegateComp::OnComponentCreated()`, one `ExecSqlQuery` call per
`CREATE INDEX IF NOT EXISTS` statement. This is required because the QSQLITE Qt
driver executes only the *first* statement of a semicolon-separated batch
passed to a single query — bundling table + indexes in one script (as PostgreSQL
allows) silently drops every index on SQLite. Mirrors the existing convention in
`imtchatdb::CMessageDbDelegateComp` for its "MessageAttachments" table.

---

## 4. Filtering, sorting & pagination (server-side)

All of it is computed on the server; the client only renders the page it is
given.

- **Filters** — read state (`unreadOnly`), producer `category`, `severity`.
- **Sort** — `NewestFirst` / `OldestFirst` (by `createdAt`).
- **Pagination** — `viewParams.offset` / `viewParams.count`; the payload returns
  `totalCount` so the client knows whether more pages exist ("Load more").

The `NotificationsListInput` carries these; `CNotificationsControllerComp`
builds an `INotificationService::ListFilter` (scoping `recipientId` to the
authenticated user) and the service applies filter/sort/pagination. Recipient
scoping and category filtering are additionally pushed to SQL through
`CNotificationDbDelegateComp::CreateAdditionalFiltersQuery`.

The UI (`NotificationFilterBar`) exposes: All / Unread, a severity segmented
filter, and a newest/oldest sort toggle — each re-issues a server query via
`ActivityController`.

---

## 5. Raising a notification from a producer

### From C++ (preferred)

```cpp
I_ASSIGN(m_notificationServiceCompPtr, "NotificationService",
         "Notification service", true, "NotificationService");
...
I_REF(imtnotify::INotificationService, m_notificationServiceCompPtr);

imtnotify::INotificationService::NotificationParams params;
params.recipientId = recipientUserId;
params.category    = "Ticket";
params.title       = "New message on #17";
params.body        = messageText;
params.iconName    = "Icons/Message";
params.severity    = imtnotify::INotification::SV_INFO;
params.sourceType  = "SupportTicket";
params.sourceId    = ticketId;
params.targetRoute = "Tickets/Ticket/" + ticketId;
m_notificationServiceCompPtr->RaiseNotification(params);
// or RaiseNotificationForRecipients(recipientIds, params);
```

### From GraphQL (integrations / tests)

```graphql
mutation {
  RaiseNotification(input: {
    recipientId: "user-42", category: "System",
    title: "Welcome!", body: "Your account is ready.", severity: Info
  }) { id }
}
```

---

## 6. Server assembly

`Impl/ImtControlsGallery/Partitura/ImtControlsGalleryVoce.arp/NotificationController.acc`
wires the whole server side and is ready to be `#include`d into the application's
server assembly. It instantiates:

1. `NotificationFactory` (`ImtNotifyPck::Notification`)
2. `NotificationDatabaseDelegate` (`ImtNotifyDbPck::NotificationDbDelegate`) —
   `DatabaseEngine` (external), `NotificationFactory`, `TableName=Notifications`,
   `AutoCreateTable=true`, `CreateTableScriptPath=CreateNotificationsTable.sql`
3. `NotificationsCollection` (`ImtDatabasePck::SqlDatabaseCollection`) →
   `ObjectDelegate = NotificationDatabaseDelegate`
4. `NotificationService` (`ImtNotifyPck::NotificationService`)
5. `NotificationsController` (`ImtNotifyGqlPck::NotificationsController`)
6. `NotificationPublisher` (`ImtNotifyGqlPck::NotificationPublisher`) →
   `RequestManager` (external), `CommandId=OnNotificationReceived`

The `NotificationsController` and `NotificationPublisher` must be added to the
server's GraphQL controller / subscriber list exactly like
`TicketCollectionController` / `TicketMessageNotifier`. All reads/writes are
scoped to the authenticated user from the GraphQL context.

---

## 7. Client wiring (done for the reference app)

- **Page** — `Pages.acc` registers page id `Events`
  (`SingleDocPage` → `qrc:/qml/imtnotifygui/ActivityPage.qml`); added to the page
  list in `ControlsGalleryHandlers.acc`. Product apps add the same two entries.
- **QML modules** — `Qml/imtnotifygui` and `Qml/imtnotifyguigql` are registered
  in `Qml/web/imtcore.json` (dirs + `imtnotifysdl` generation info) and in the
  web build via `getImtNotifyQmlWebDirs` in `Config/CMake/WebCompiler.cmake`
  (called from `getImtCoreQmlWebDirs`), which adds both QML dirs and the
  generated `imtnotifyNotificationsSdl` module to the web bundle.
- **Transport injection** — `ApplicationMain.qml` creates a
  `GqlNotificationApiClient`, assigns it to `ActivityController.apiClient`, then
  calls `ActivityController.start()` on login / `stop()` on logout.
- **Indicator** — `ActivityController` sends `UpdatePageBadge { id: "Events",
  count }`; `MenuPanel` maps it to `MenuPanelButton.badgeCount` and
  `MenuPanelButtonDecorator` renders a pulsing badge on the icon.
- **Popup** — a live `OnNotificationReceived` payload raises a
  `NotificationPopup` via `PopupManager`; clicking marks it read and deep-links
  to the source (see below).

### Navigating to the source entity

Clicking a notification (row or popup) deep-links to the producing entity using
the **same mechanism as the global Search** (`SearchPage.qml`): the
notification's `targetRoute` is the destination path and the source entity is
passed as navigation params, so the destination page opens exactly the right
document:

```qml
NavigationController.navigate(targetRoute, {
    "contextId":     sourceId,     // e.g. the ticket id
    "contextTypeId": sourceType,   // e.g. "SupportTicket"
    "resultName":    title
})
```

A producer therefore sets `targetRoute` to the page path (e.g. `"Tickets"`),
`sourceType` to the entity type and `sourceId` to the entity id when calling
`RaiseNotification`. Any page that already supports Search navigation supports
notification navigation for free. These fields flow end-to-end: producer →
`INotification` → DB → `NotificationItemData` / `NotificationPush` → QML.

---

## 8. Build / code generation

All targets are registered in both master build files —
`Build/CMake/CMakeLists.txt` and `Build/QMake/ImtCoreAll.pro` — next to their
`imtchat` / `imtdesk` siblings:

- SDL: `Sdl/imtnotify` (generates C++ **and** QML from `Notifications.sdl`)
- Libs: `Include/imtnotify`, `Include/imtnotifydb`, `Include/imtnotifygql`
- Packages: `Impl/ImtNotifyPck`, `Impl/ImtNotifyDbPck`, `Impl/ImtNotifyGqlPck`
- QML: `Qml/imtnotifygui`, `Qml/imtnotifyguigql`
- Test: `Tests/NotificationTest` (CMake only, like `TicketServiceTest`)

Run the SDL code generator for `Notifications.sdl` before building the C++/QML
that consume `GeneratedFiles/imtnotifysdl/...` and the `imtnotifyNotificationsSdl`
QML module.

---

## 9. Tests

`Tests/NotificationTest` covers the `CNotificationComp` `IChangeable` contract
(CopyFrom / IsEqual / CloneMe / ResetData) and accessors. Service-level filter /
sort / pagination and GraphQL scoping tests should be added next.
