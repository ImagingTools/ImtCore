# Notification Center

The notification center is the one place where a user receives the notifications of all features of the client.
A notification is stored, so a user who is offline sees it after the next login.

## Rules

- A notification belongs to the **inbox of one user in one tenant and product**. The server takes the user, the tenant and the product from the request context, never from the input.
- A **personal** notification is sent without a tenant (and/or without a product). It is visible in every tenant (product) of the user, e.g. an invitation to a tenant the user has not joined yet, or a ticket, which belongs to no tenant.
- A **notification** is something the user must know about. A **signal** ("data changed, reload") is not a notification and keeps its own subscription (`OnMembershipNotification`, `OnOrganizationMembersChanged`, `On*CollectionChanged`, ...).
- A notification is `imtnotify::INotification`, implemented by `CNotification`, an `ilog::CMessage`: its category is the severity (`IC_INFO` … `IC_CRITICAL`), its description the text a user wrote (e.g. the beginning of a message), its source the name of the newest actor, its time stamp the time of the latest event. On top of the message it has the inbox (user, tenant, product), the type (`typeId`), the subject the client shows and opens, the actors (who caused the latest events of a group, the newest first, at most three), the arguments (the values the message of the type puts in), the group key, the group count and the state (delivered, read). Its id is its id in the collection. The dedup keys are kept apart (see Storage).
- **No text in a language.** A notification carries facts: its type, its subject, its actors and its arguments (`imtnotify::NotificationArgument`: an id, a kind — `Text`, `Number`, `DateTime`, `Object` — and the value; an object also has its id and type). The **message** of its type puts them into a sentence in the language of the client (`{member} is no longer a member of "{subject}"`), below it the label of the type and the actors, with the icon of the type and the time. Names and texts are snapshots taken when the event happened, data that is never translated. The labels and messages are the only words to translate: the **catalog of the types** (`NotificationTypes`) gives them, in the language of the client, from the server the client talks to. Puma stores and delivers facts and knows no language; a product server forwards the inbox as it is and answers the catalog itself.

## Where it runs

There is **one** notification center: on the authorization server (Puma). A product server forwards the inbox commands of its clients and relays the subscription, as it does for the tenant commands, and sends its own notifications to Puma with the account of the server.

```
                         Puma (authorization server)
   organization events ──INotificationSender──► CNotificationSenderComp ──► INotificationInboxController ──► table UserNotifications
                                                   ▲          │ every change              (collection Notifications reads it)
     SendNotification (session of the user) ──────┤          ▼
     inbox commands (session of the user) ────────┘   CNotificationPublisherComp ──OnNotificationEvent──┐
                                                                                                         │
                         Product server (ProLife, ...)                                                   │
   ticket events ──► CRemoteNotificationSenderComp ──SendNotification──► Puma                           │
   client inbox ───► CRemoteNotificationsControllerComp ──forward──► Puma                               │
   collection "Notifications" ──► RemoteObjectCollectionController / list forwarder ──► Puma            │
   client subscription ◄── PublisherSubscriberBridge ◄──────────────────────────────────────────────────┘
```

## Modules

| Module | Content |
|---|---|
| `Sdl/imtnotify` | `Notifications.sdl`: `UserNotification`, inbox queries and mutations, `SendNotification` / `WithdrawNotifications`, the `OnNotificationEvent` subscription, the collection `Notification` (list `UserNotificationsList`) |
| `Include/imtnotify` | `INotification` / `CNotification` (the notification, an `ilog::CMessage`), `INotificationSender` (features; implemented by `CNotificationSenderComp` on Puma and the remote sender of a product server), `INotificationInboxController` (the only writer of the inboxes: add, group, dedup, read, delivered, remove, cleanup), `CNotificationRetentionComp` (removes the expired notifications and those of removed users), `NotificationSelection.h` (selection of an inbox) |
| `Include/imtnotifydb` | `CSqlNotificationInboxControllerComp` (the `INotificationInboxController` on PostgreSQL and SQLite, it publishes every change), `CNotificationDbDelegateComp` (the read-only delegate of the SQL collection `Notifications`), `NotificationRecord.h` (table and columns) |
| `Include/imtnotifygql` | `CNotificationsControllerComp`, `CNotificationCollectionControllerComp`, `CNotificationPublisherComp` (Puma); `CRemoteNotificationsControllerComp`, `CRemoteNotificationSenderComp` (product servers) |
| `Impl/ImtNotify*Pck` | Component packages |
| `Partitura/ImtCoreVoce.arp` | `NotificationCenter.acc` (Puma, with the collection `Notifications`), `RemoteNotificationCenter.acc` (product servers, part of `PumaGQLClientEngine`, forwards the collection too) |
| `Qml/imtnotifygui` | `NotificationCenter`, `NotificationTypeRegistry`, `GqlBasedNotificationApiClient`, `NotificationBell`, `NotificationPanel`, `NotificationsPage` (a `RemoteCollectionView`), `NotificationDelegate` |
| `Include/imtcore/CImtCoreNotifyInitializer.h` | Resource initialization of the SQL scripts (Puma) and the QML modules (clients) |

## Producers

| Producer | Notifications | Inbox |
|---|---|---|
| `imtauthgql::CTenantNotificationPublisherComp` | invitation received / accepted / declined / withdrawn, ownership, permissions changed, membership ended, member left | invitation and end of membership: personal; others: the tenant in every product (no product is set) |
| `imtauthgql::CConnectionCodesNotificationPublisherComp` | connection requests and relationship proposals received / approved / rejected, connection and relationship removed | the tenant of the owner |
| `imtauthgql::CCrossTenantMessageNotificationPublisherComp` | new message from another tenant | the target tenant |
| `imtdeskgql::CTicketNotificationProducerComp` | new message of a ticket (reporter and assignees but the sender), assigned to a ticket | personal in the product |

Types: a module declares each of its notification types as an element `NotificationType` (`ImtNotifyGqlPck`, `imtnotify::INotificationTypeInfo`) with `TypeId`, `Name` (the label) and `IconPath`, and each type of the objects of its notifications as a `NotificationType` without an icon ("Organization" for `Tenant`, "Ticket" for `SupportTicket`). A `NotificationTypeList` (`imtnotify::INotificationTypeList`) lists them in `Types` and `ObjectTypes`; the composite of the module exports it: `ImtUserAdministrationVoce/TenantNotificationTypes` for the organizations, `ImtDeskVoce/TicketNotificationTypes` for the tickets. The labels are texts of the `.acc` file: their translations are in the `.ts` of the module, context `Attribute`, and the server translates them into the language of the client. A server lists the type lists of the modules it runs in `NotificationTypes` of its notifications controller: Puma the organization types; a product server always knows the organization types (inside `RemoteNotificationCenter`, not exported), a product adds only its own lists through `AuthorizableServerFramework/ProductNotificationTypes` (ProLife: the tickets). A client shows a notification as `Organization "Test"` with the label of its type. A product with its own notification type adds its list there; neither Puma nor ImtCore change.

Messages: a type may have a `Message`, a text of the `.acc` file translated like its label, in the subset of ICU MessageFormat of `imtnotify::NotificationMessageFormat`: `{id}` puts in a value; `{id, plural, =1 {…} one {…} few {…} many {…} other {…}}` chooses by a number in the language of the catalog (`#` is the number); `{id, select, value {…} other {…}}` chooses by a text, e.g. a status; `''` is an apostrophe and an apostrophe before `{`, `}` or `#` quotes. The standard values are `subject` (the name of the subject), `actor` (the newest actor), `actors` (the names of the latest actors) and `count` (the events of a group); the type declares the others in `ParameterIds` (e.g. `partner`, the other organization of a connection, `member`, the user an organization notification is about), and a producer sends them in `INotificationSender::Message::arguments`. A message that is not valid or uses an undeclared value is logged when the type is created; a translation that is not valid or uses other values than its message is replaced by the message when the catalog is built, and logged once. Ids and branch names are ASCII letters, digits and `_` on the server and in the client alike. The client renders the message (`Qml/imtnotifygui/NotificationMessageFormat.js`); without a message, or when a value is missing, it shows the subject with the type of its object (`Organization "Test"`). The actor never receives the notification about the own action, so a user the notification is about but who did not act is an argument (`member`), not the actor. Russian has no gender-free past tense for people: the Russian messages are worded without it (`Приглашение в «{subject}» принято: {actor}`).

The producers observe their models, so they have to exist from the start of the server: they are exported up to the server registry and listed in its `ComponentsToInitialize` (Puma: `PumaServerBase/TenantNotificationPublisher`, `.../ConnectionCodesNotificationPublisher`, `.../CrossTenantMessageNotificationPublisher`; ProLife: `ProLifeServerBase/TicketNotificationProducer`).

## Lifecycle of a notification

```
Created ──► Delivered ──► Read ──► removed by the user or by retention
   ├──► grouped: an unread notification with the same group key is updated (groupCount + 1, new again)
   └──► withdrawn: INotificationSender::Withdraw(product, tenant, groupKey) removes it from every inbox
```

- **Delivered** means a session has received it (push or sync). Undelivered notifications pop up at the next login.
- **Read** implies delivered.
- A second `Send` with the same **dedup key** in the same inbox is ignored, also after the first one was grouped, read or removed by the user. Withdrawing a group forgets the dedup keys equal to its group key, so the event can be told again.
- The **actor** never receives a notification about the own action.

## Sending

### On Puma

```cpp
I_ASSIGN(m_notificationSenderCompPtr, "NotificationSender", "Notification sender", false, "NotificationSender");
I_REF(imtnotify::INotificationSender, m_notificationSenderCompPtr);

imtnotify::INotificationSender::Audience audience;
audience.tenantId = tenantId;        // empty: personal
audience.userIds = {userId};

imtnotify::INotificationSender::Message message;
message.typeId = "imtauth.InvitationReceived";
message.subject = {tenantId, "Tenant", tenantName};
message.severity = istd::IInformationProvider::IC_INFO;
message.groupKey = "invitation:" + invitationId;

m_notificationSenderCompPtr->Send(audience, message);
```

### On a product server

The same interface: `PumaGQLClientEngine` exports `NotificationSender` (the remote sender). It sends `SendNotification` to Puma with the **session of the user whose request made the change**; a change made outside of a user request is not sent and a warning is logged.

- Puma accepts `SendNotification` and `WithdrawNotifications` from any signed-in user, only for the product of the request and for at most 100 users. With a tenant, every recipient must be a member of it, otherwise nothing is sent.
- **Known hole:** a product server has no identity of its own yet, so any signed-in user of a product can send or withdraw notifications of that product, with any actor. The fix is a product server account, ImagingTools/ImtCore#1059.
- The sender checks the lengths: the type and the ids of the objects at most 100 characters, the group and dedup keys at most 200; longer names and `Text`/`Object` values are cut to 200 and the body to 500, never in the middle of a surrogate pair; a `Number` or `DateTime` value longer than 200 is refused, since cutting it would change it. It refuses more than 10 arguments, an argument id other than ASCII letters, digits and `_`, a repeated one, one of the standard values (`subject`, `actor`, `actors`, `count`), a `Number` that is no number and a `DateTime` that is no ISO 8601 time.

Send after the business change has succeeded. On a product server sending is queued: the call returns at once and an own thread of `CRemoteNotificationSenderComp` delivers the requests one by one, in their order (at most 1000 wait). A request Puma did not answer is repeated up to five times every 30 seconds, then dropped; a refused one (GraphQL error) is dropped at once. Both are logged and do not cancel the change. A notification without a dedup key gets `request:<uuid>`, so a repeated request does not deliver it twice.

## Server wiring

**Puma** (`PumaServerBase.acc`): element `NotificationCenter` (`ImtCoreVoce`) with `DatabaseEngine`, `TenantMembershipManager` (the membership check of `SendNotification`) and `UserCollection` (the notifications of a removed user are removed); it is a GraphQL handler (`PumaHandlers/ExternGqlRequestHandlers`) and a subscriber controller (`WebSocketServerFramework/GqlSubscriberControllers`); `TenantController/NotificationSender` points at `NotificationCenter/NotificationSender` and `TenantController/NotificationInboxController` at `NotificationCenter/NotificationInboxController` (a removed member loses the notifications of the organization). The servers call `ImtCoreInitNotifySqlResources()` and link `ImtCore::imtnotifydb`. The composite also serves the collection `Notifications`: `CNotificationCollectionControllerComp` over a `SqlDatabaseCollection` with `CNotificationDbDelegateComp`, headers `typeId` (the subject and the type label), `severity`, `createdAt`.

**Product servers**: nothing to add for the inbox — `AuthorizableServerFramework` contains it. Set `ProductId` (already set by every product). `OnNotificationEvent` is relayed by `RemoteTenantController`; the commands of the collection `Notifications` (`UserNotificationsList`, `GetCollectionHeaders`, ...) are forwarded by `RemoteNotificationCenter`.

A `.acc` registry is compiled into the executable: re-run CMake after a change.

## Client wiring

Call `ImtCoreInitNotifyQmlResources()` in `main()` of the client and link `imtnotifysdl` and `imtnotifyguiqml`, then inject the transport once and register the routes of the product:

```qml
import imtnotifygui 1.0

property GqlBasedNotificationApiClient notificationApiClient: GqlBasedNotificationApiClient {}

Component.onCompleted: {
	NotificationTypeRegistry.registerRoute("SupportTicket", "Tickets/Ticket/{id}")
	NotificationTypeRegistry.registerRoute("Tenant", "Tenants/Tenant/{id}")
	NotificationCenter.apiClient = notificationApiClient
}
```

- `NotificationCenter` starts at login, reloads and resubscribes at a tenant switch and stops at logout. Every request of `GqlBasedNotificationApiClient` is an object of its own, destroyed with its answer; an answer to a request sent before the last stop, e.g. in the previous tenant, is dropped.
- A pushed change of a personal notification (unread count -1) makes the session ask `NotificationsUnreadCount`, one count in the storage; changes that come together ask once.
- A new notification pops up as a standard message of `PopupManager` (its kind by the severity): the message of its type, the label and icon of its type and the actors; a click opens it. More than three undelivered notifications at login give one summary message with their count that opens the panel; they are acknowledged at once with `AcknowledgeAllNotificationsDelivered` up to `syncedAt` of the sync. The popups are closed at logout.
- The panel closes with Escape; a row of the panel gets the focus with Tab and opens with Enter or Space.
- The standard top-right panel shows `NotificationBell` while the center runs. A product with its own top-right decorator places `NotificationBell {}` itself.
- `NotificationsPage` is the page `Notifications` of `PagesController.acc`, hidden by default; a product shows it with `NotificationsPageVisible` of `ApplicationHandlers` and sets `NotificationCenter.pageRoute = "Notifications"` for "All notifications" in the panel.
- A page that shows an object calls `NotificationCenter.setOpenedSubject(typeId, id)` (and empty values when it closes it): new notifications about it are read at once and do not pop up.
- A click marks the notification read and calls `NavigationController.navigate(route, {contextId, contextTypeId})`.

## Notifications page

`NotificationsPage` shows the whole collection of the user in the current tenant and product. The list is the ordinary server collection `Notifications` in a `RemoteCollectionView`:

- search, the filters State (unread / read), Severity, Type and the date filter of the filter panel, sorting by the columns, paging — all done by the server;
- the unread count as a badge on the menu entry of the page (`UpdatePageVisualStatus` with `badge`) and on `NotificationBell`;
- the commands come from the server like those of every collection: `CommandsController` with `TypeId` `Notifications` in `NotificationCenter.acc` (`MarkAsRead`, the standard `Remove`, `MarkAllAsRead`), forwarded by `RemoteCommandsController` on a product server. The page enables them by the selection and the unread count; `Remove` is the standard `RemoveElements` of the collection, which `CNotificationCollectionControllerComp` does through the inbox of the caller, so the sessions learn it;
- a double click marks the notification read and opens its object, as a click in the panel does;
- the page reads the collection again on `NotificationCenter.inboxChanged` (a pushed event, a change of this session, a reconnect); a hidden page does it when it is shown again.

The collection is read-only and always restricted to the inbox of the caller:

- `CNotificationCollectionControllerComp` refuses an anonymous request and puts the user, the tenant and the product of the request context into the selection parameters `imtnotify::s_inbox*ParamId`.
- `CNotificationDbDelegateComp` turns them into the inbox condition of the storage (own inbox and the personal notifications, without them the selection is empty. The collection controller serves only the list, the headers and `RemoveElements`, so a notification is never read by its id from a client.
- Only the fields of `imtnotify::NotificationField` can be filtered; sorting by `createdAt` and `severity`, the newest first among equal values. Any other field or operation selects nothing, so a column name never comes from a request. A filter is at most 4 levels deep, a search at most 10 words, a selection at most the 1000 newest notifications.
- The text filter needs every word in `SearchText`, `%` and `_` are plain characters. The periods of the date filter are counted in UTC.

The options of the Type filter are the types of the catalog, with their labels in the language of the client.

## Storage

- Table `UserNotifications`, created by the delegate (`AutoCreateTable`). `IX_UserNotifications_Inbox` (`RecipientId`, `CreatedAt`, `Id`) is the order of the pages, so a page reads its rows only; `IX_UserNotifications_Undelivered` the same for the undelivered ones, `IX_UserNotifications_Unread` the unread count, `UIX_UserNotifications_OpenGroup` the open group. There is no index of the time alone: the planner preferred it for the pages of a user with old notifications and filtered the rows of every other user; the retention reads the table once per check instead. The actors and the arguments are JSON text in the columns `Actors` and `Arguments`: they are only shown, never selected by, and a group replaces them in the same `UPDATE`; their names and texts are in `SearchText`, so the search finds them. A group merges its actors with the ones it read and updates only if they did not change meanwhile, else it reads them again. Its content (type, subject, arguments, body, time) is the one of its latest event: an earlier event that is stored later only adds to the count and the actors.
- Table `UserNotificationDedupKeys` (product, tenant, recipient, dedup key, creation time): a dedup key is claimed with `INSERT … ON CONFLICT DO NOTHING` before the notification is stored, so two concurrent sends of the same event store one notification.
- `SearchText` keeps the searchable text lowered by Qt: SQLite lowers only ASCII. The search compares it with `LIKE ... ESCAPE`, the same in both databases.
- Times are ISO 8601 UTC text in both databases, so they compare and sort the same way. Ids are text: a non-UUID value in a UUID column is an error on PostgreSQL.
- Every page and count is computed in SQL. Read, delivered and remove changes are one `UPDATE`/`DELETE … RETURNING`. Marking all read or delivered answers no ids; the event `AllRead` carries `createdUntil`, the time up to which the notifications of the inbox are read. It goes only to the sessions of that tenant and product; the notifications of a personal inbox, seen in sessions of other inboxes too, come as `Read` with their ids, so no session marks a notification read that was not.
- A dedup key, the event and the group are stored in one transaction (`INotificationInboxController::Store`), and the sessions learn the change after the commit: a stop between them neither loses the event nor stores it twice.
- `CNotificationRetentionComp` removes the notifications and dedup keys older than `RetentionDays` (90) every `CheckIntervalHours` (6), the first time a minute after the start, in portions of 500 with pauses, so the thread keeps serving; a portion removes its keys and notifications in one transaction, and the sessions learn the removals after its commit. The notifications of a removed user, and those of an organization a member left (`CTenantNotificationPublisherComp`), are removed with their dedup keys in one transaction; a failed removal is logged and retried, by the retention with its next check and by the publisher every minute.
- Withdrawing a group repeats `"GroupKey" <> ''` of the partial index `IX_UserNotifications_Group`, so a prepared statement uses it; the dedup keys have indexes by key (`IX_UserNotificationDedupKeys_Key`) and by recipient (`IX_UserNotificationDedupKeys_Recipient`).

## Tests

- `Puma/Tests/TenantApiPostman/Notification_Center.postman_collection.json` and `ws/ws-notification-center.js` — the API and the push of Puma: access, validation, inboxes, groups, dedup, withdrawal, inbox operations, catalog, notifications page and the notifications of the organization events.
- `ProLife/Tests/ProLifeApiPostman/Notification_Center_ProLife.postman_collection.json` — the forwarded inbox, the catalog of the product and the ticket notifications.

## Known limitations

- The names of the subject, the actors and the arguments are snapshots taken when the event happened: a renamed organization keeps its old name in the old notifications. The objects live on different servers, reading them when shown would ask every server for every row.
- The client knows the plural rules of Russian, Ukrainian and Belarusian; every other language gets the rule of English (one, other).
- A label is translated by the server of the client; a client whose server lacks a type shows its id. The client loads the catalog at login and again after a change of the language (`OnLocalizationChanged`).
- The inbox controller uses `UPDATE`/`DELETE … RETURNING`: PostgreSQL, or SQLite 3.35 and newer (bundled with Qt 6). The SQLite of Qt 5.15 is older.
- The periods "current" and "last" of the date filter are counted in UTC, not in the time zone of the user.
- The previous assignees of a ticket are known only since the start of the server; after a restart the dedup key keeps an assignee from being told twice.
- A personal notification sent by a product server is not checked against the members of a tenant: it has none.
- The push carries the notification itself: a mistake in the WebSocket routing would leak it. The publisher filters by user, tenant and product.
