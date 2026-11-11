# Server tokens between product servers

A product server that calls another product server proves who it is with a server token.
The receiving server issues the token for itself only and keeps it.
The calling server sends the token in the header `x-server-token` with its product in `productid`.

```
Operator (Agentino or the Puma console)        Receiving server (Puma, Lisa, ...)           Calling server (ProLife, ...)
   | Authorization (superuser of the receiver) --> session
   | AddServerTokenClient (clientProductId)    --> the product becomes a client (no change if it is one)
   | GetServerTokenReceiver (clientProductId)  --> permission tree the token can hold
   | CreateServerToken / UpdateServerTokenScopes / RevokeServerToken
   |                                             <-- token value (only at creation)
   | the value reaches the calling server (Agentino passes it in IMT_<RECEIVER>_SERVER_CREDENTIAL)
   |                                                                                            | request
   |                                             <------------------ x-server-token, productid
   |                                             AuthenticationManager: token issued here, for this client
   |                                             the handlers check the request with the permissions of the token
```

## What a token allows

A token holds permissions of the permission tree of the receiver, the same permissions its roles are made of
(Puma: `PumaFeatures.h`, Lisa: `LisaFeatures.h`). There is no list of the commands a server may send:
the handlers of the receiver check a request through a server as they check any request.

- **A server acting on its own** (its token in the authentication header) has an explicit service principal
  (`IGqlContext::PK_SERVICE`). Its user-info representation contains only the permissions of its token.
- **A request a server forwards for its user** (the session of the user and `x-server-token`) holds the permissions of the user
  that the token holds too. A superuser forwarded by a server holds the permissions of the token and is no superuser there.
- **What a connected product does for itself and its users needs no permission:** signing its users in, their sessions and
  profiles, their notifications and personal tokens, publishing its permission tree, sending notifications to its own users.
  The handlers limit these to the product of the server and to the user of the request.
- The permissions of a member inside an organization are a mechanism of their own and are not bounded by the token.

User permissions include feature requirements **before** they are intersected with token scopes.
For example, a user holding `Edit` that requires `View` can use a `View`-only token.
The resulting context retains an explicit permission boundary: resolving requirements later cannot restore
permissions outside that boundary. Combining a personal and a server token intersects both boundaries;
an explicitly empty boundary grants no product permissions. Copying a context preserves its principal and boundary.

## Token storage and automatic migration

`CPersonalAccessTokenDatabaseDelegateComp::CreateTableIfNeeded()` creates a missing table and automatically
migrates an existing `PersonalAccessTokens` table during component initialization. No separate migration command is needed.
The migration adds `ClientProductId`, makes `UserId` nullable for service tokens and ensures token-hash and
client-product indexes exist. It is transactional and repeatable.

SQLite requires a table rebuild to remove the owner constraint. Rows, table constraints, explicit indexes and
triggers are preserved. An unsupported legacy table definition, duplicate token hashes, or incoming foreign keys
causes an explicit error and rollback instead of silently discarding data. In particular, incoming foreign keys require
an explicit migration designed for that schema, to avoid `ON DELETE CASCADE` during the rebuild.
The delegate refuses token reads and mutations when schema initialization fails.

The standard `PersonalAccessTokenStore` connects `TokenPersistence` to the SQL delegate.
Revocation, scope changes and usage timestamps use field-specific SQL updates through
`IPersonalAccessTokenPersistence`; they do not save an earlier copy of the complete token.
Usage updates cannot broaden scopes or undo revocation, timestamps cannot move backwards,
and scope changes on revoked tokens are rejected.
An empty scope list is valid and grants no product permissions. Scope updates store it as a non-null empty
string, including repeated updates to an already empty list.
Custom persistent registries must wire this interface too. `InMemoryUpdates=true` is an explicit opt-in
only for a private in-memory collection; its read/modify/write operations are serialized by the manager.

Rebuild dependent products and component packages together when deploying these changes.
The C++ context/storage/client interfaces changed; old binaries must not be mixed with the rebuilt packages.
Rebuild every ImtCore library imported by the products before relinking their executables, including
indirect context consumers such as `imtdeskgql`. Product builds can link prebuilt imported libraries
without rebuilding them; a successful executable link alone does not establish ABI consistency.
Do not rebuild shared static libraries concurrently with product linkers that read them.

Local token-change notifications carry the token ID. Authentication removes the matching cached entry rather than
all PAT entries, and a generation check prevents an in-flight lookup from restoring an invalidated entry.
Unidentified change notifications still invalidate the whole PAT cache. These notifications are process-local;
they are not a distributed cache invalidation protocol.

## WebSocket credential lifetime

A subscription keeps, on the server, its registration headers and an immutable value snapshot of its access
(`imtservergql::AuthorizationData`): principal, user, organization,
products, administrator and owner flags, groups, effective permissions, organization and tenant permissions,
token boundaries and granting authority.
Access is compared structurally; there is no production string fingerprint.

A subscription with credentials is built with `IGqlContextCreator::RenewGqlContext`, never from the caches of the requests:

| Context | Caches | Expired access token |
|---|---|---|
| A request (`CreateGqlContext`) | Used | Refused |
| A new subscription (`RenewGqlContext`, not established) | Not used | Refused with `CCS_UNAUTHORIZED`: the client refreshes it |
| An established subscription (`RenewGqlContext`, established) | Not used | Accepted while its session lives |

A new subscription is registered at its publisher with the context of this check only, so revoked credentials never become a
subscription. If the authorization generation changed during registration, a second check must confirm that the original
access is still served, using the same rules as an established subscription;
otherwise the registration is rolled back without a success acknowledgement. A check ends as one of:

| State | Action |
|---|---|
| `AS_CONFIRMED` | Keep the subscription if `IsAccessWithin(registered, current)` holds, or if its explicitly declared recipient-only channel still has the same recipient; otherwise close it |
| `AS_DENIED` | Close the subscription |
| `AS_UNAVAILABLE` | Keep the subscription within the grace period; nothing is known (a failed read is not a removed account, session, role, group, organization or membership) |
| `AS_STALE` | The access changed during the check; the result is not used and the check is repeated, within the grace period |

`IsAccessWithin` compares with the **registration**, not the previous check. Additional permissions or groups do not end the
subscription: `{A}` -> `{A,B}` -> `{A}` still serves a registration with `{A}`. Its publisher keeps the registration context;
new access becomes available to that subscription only after registering again. Ordinary requests still use current access.
Identity, scope, administrator and owner flags must match, and every registered permission set must be contained in the
current set. Appearing or disappearing token boundaries or granting authority are treated conservatively as incompatible;
an absent boundary is not a boundary with no allowed permissions. A change of granting-administrator status is incompatible too.

`IGqlSubscriberController::SK_RECIPIENT` explicitly declares an authenticated recipient channel independent of permission sets.
It is false by default and is checked for the specific command, separately from ordinary user/admin admission.
Only the personal membership, connection-code and cross-tenant-message commands declare it; organization-member data,
collection changes, connection status and the scope-filtered notification center do not. `IsSameRecipient` requires the same
principal, user, organization, product and server product, but not the administrator/owner flags. A confirmed permission
reduction therefore leaves a personal channel alive while closing incompatible data subscriptions without a delay window.
Credential refusal, a changed recipient or exhausted grace still ends the personal subscription.

Personal notifications about invitations and ended membership are also stored in the notification center with a personal
scope, so the recipient can read them after reconnecting without membership. This recovery is additional to live delivery,
not a substitute for delivering the notification on a surviving personal channel.
Collection-cache invalidation must continue for objects read after access expands; keeping a safe subset of old data is
not sufficient if ordinary reads would return incomplete or stale results.

### Publisher admission and collection notifications

`CGqlPublisherCompBase.ReadPermissions` is optional and applies before registration, initial delivery or upstream
registration by a derived publisher. A configured channel requires a user context and at least one of the listed
permissions, using the existing effective-user/organization permission check and administrator exception.
An empty list preserves the publisher's admission and recipient filtering; it does not make a channel anonymous.
The collection notifier inherits this check instead of maintaining its own copy. Existing collection permission
configuration remains unchanged.

A personal publisher with configured `ReadPermissions` no longer declares `SK_RECIPIENT`: its admission now depends
on permissions, so narrowing cannot retain it using the identity-only exception. Similarly, connection status is
anonymous only without configured read permissions. Its default login-page behavior is unchanged.

Generic collection-change events contain `typeOperation` and `itemId` (insert/update) or `itemIds` (remove).
An optional `operationContext` contains only `ownerId`, needed to distinguish local from remote edits.
Neither object contents nor human-readable object/owner names are sent. `IsSendItemSource` is removed;
clients read object data using an ordinary authorized request. Document-service and personal-notification payloads
are separate contracts and are not changed by this restriction.

### Recovering a collection subscription

Loss of a confirmed upstream collection subscription marks the remote collection unwatched. Its cache is invalidated
and reads bypass the cache until all required subscriptions are acknowledged again. The watched transition also
invalidates the cache, before reuse. These cache changes notify product-servlet `AccessModels`, scheduling downstream
authorization checks rather than waiting for the 300-second control interval. Existing unavailable/stale retry limits
and grace still apply; reconnect is not authorization confirmation.

The QML client requests a state refresh for an interrupted subscription only after `start_ack`,
not when sending `start`. Sending the WebSocket registration before an HTTP read does not guarantee
that registration has completed. Failed registration attempts retain the interruption flag until an
acknowledged retry can request the refresh.

The servlet tracks access-model events during registration as well as token invalidations.
An event after reading access triggers a registration recheck; an event after that recheck but
before insertion into the schedule schedules that credential key for another check. Insertion
and event accounting share the same lock, so the first subscription cannot miss the event merely
because its schedule entry did not exist yet.

The component regression exercises the real collection subscriber, cache model and servlet at a 300-second interval:
cache hits before loss, fresh uncached reads during the gap, and downstream revocation after re-acknowledgement.
Transport status changes are injected in this test; it is not evidence of a live network interruption.

### The user computed by its source

A product server whose users live at an authorization server (`AuthorizableServerFramework`: Puma for ProLife and Lisa) sets
`IsUserComputedBySource` of its authentication manager. The authorization server then hands out the user for the product `P` and
the organization `T` of the request, with its final permission set in `permissions[P]`:

1. Puma reads the user, its roles and groups at the sources of their collections (`imtservergql::CAuthorizationSnapshot`, see below),
   and the bindings, memberships and grants with their read status.
2. It adapts the user to `T` and resolves the permissions with the usual tenant-aware code (`AdaptUserForTenant`,
   `GetTenantUserPermissions` in `imtauthgql.h`): a role, a group, a parent group or an included role not visible in `T` grants nothing,
   delegated roles are added.
3. Only when every read was complete it writes `permissions[P]`, an empty set too, and lets the user point to no provider.
   A role, a group, a binding, a membership or a grant that could not be read gives no user: `GetObjectData` answers with an error.

The product server keeps the identity, the administrator flag, the enabled state and the ids of the roles and groups as data, but
takes only `permissions[P]`: it does not expand roles or groups itself, and its copy points to no provider. Such a user is valid for
one product and one organization, so the shared user cache, keyed by the user id, may not serve it: the user is read at the source of
the collection for every request and every subscription check. Personal tokens keep their existing credential-resolution path.

### Session authorization between servers

For session JWTs, `SessionAuthorizationProvider` on the product authentication manager calls `GetSessionAuthorization`.
On Puma, the existing `CGqlJwtSessionControllerComp` computes the local result and exposes the GraphQL operation.
The existing `CRemoteJwtSessionControllerComp` handles transport and response validation on the product server.
Both expose the additional `ISessionAuthorizationProvider` interface; `IJwtSessionController` is unchanged.
Shared session-token helpers live beside the existing JWT utilities in `imtauth.h`.
The common fresh tenant-user computation lives beside the tenant adaptation helpers in `imtauthgql.h`.
There are no dedicated session-authorization components or composition wrapper. The operation belongs to `Sessions.sdl`;
its wire name and payload are unchanged. The exported `SessionAuthorizationProvider` alias now points to the existing
remote JWT controller.

Puma wires `AuthorizationSessionController` to the raw JWT controller for fresh authorization, never to a cached
authentication result. Ordinary session operations retain their separate `JwtSessionController` reference to the
authentication manager, preserving refresh/logout invalidation. A missing fresh source produces `UNCHECKED`, not a
fallback to the ordinary controller. The browser-facing `CJwtSessionControllerProxyComp` explicitly excludes this operation
and never forwards it using the product's server credentials.
The request authenticates with `x-server-token` only; the user JWT is an input in the body, never a URL parameter or an
authentication header. API client diagnostics log the command and request id, not the query containing the credential.
HTTP worker diagnostics also omit request bodies.
The owner checks the server token afresh and binds the calculation to its client product. The session JWT is checked
with the common session-token validator, and its user, organization and originating product must match the stored session.
A session opened directly at the authorization server cannot be used through a product server.

The `isEstablishedSubscription` input is set by the product server only for an established subscription check.
Only that operation may accept an expired access JWT, and only while its session is confirmed alive. Ordinary HTTP
requests and new subscriptions use the same operation without that exception. No general HTTP authentication rule is
relaxed, and this command cannot execute another operation with the checked JWT.

One complete response supplies the computed user and its organization's owner, creator and permission scope.
The serialized user's object UUID must match the session and response user identifiers; its login name is a separate value.
The product server does not read the remote user or organization again using the access JWT. Its existing organization
semantics are unchanged: owners and creators have organization-management permissions, ordinary members are not assigned
Puma's membership permissions. An empty organization is valid; a missing organization is refused, while an unreadable
organization or incomplete permission graph is unchecked. Existing membership and delegation checks inside the user
calculation still apply.

| Result | Request | Subscription check |
|---|---|---|
| `COMPUTED`, including an empty permission set | Use the complete result | Confirm the context |
| `REFUSED / REAUTHENTICATE` | 401: refresh authentication | Denied |
| `REFUSED / FORBIDDEN` | 403: access refused | Denied |
| `UNCHECKED`, transport failure or incomplete response | Internal error, not a permission refusal | Unavailable |

An explicit HTTP 401/403 refusal of the server credentials is `REFUSED / FORBIDDEN`, not a network outage and not a
request to refresh the user's JWT. The synchronous client and asynchronous-to-synchronous adapter preserve this distinction in the common
`IGqlClient::GqlResult`: `responsePtr`, `errorCategory` and `errorMessage`. `SendRequest` has one pure virtual
signature, returning that result; there is no access-denial output flag or default implementation that loses
the category. `IAsyncGqlClient` aliases the same result and error types and returns `QFuture<GqlResult>`.

Callers check the result before using its response. A raw received GraphQL error is still a transport success;
the shared model parser reports it as `EC_SERVER_ERROR`, malformed responses as `EC_INVALID_RESPONSE`.
The synchronous `SendModelRequest(request)` returns the common `GqlModelResult<T>`. The existing
`SendModelRequest(request, errorMessage)` convenience overload remains for controllers whose public contract
already reports errors as text; it delegates to the typed path and does not suppress failures.
Authorization consumes the typed result directly. Remote collection reads translate it at the existing
collection read-status boundary, without passing transport-specific boolean pointers internally.

This changes the C++ return type of synchronous `SendRequest`: implementations and direct callers must be
updated and rebuilt together. It does not change the GraphQL wire format or asynchronous call signatures.
Notification delivery treats explicit transport access refusal as refused, rather than retrying it as
an unanswered request. File-service and token-issuer diagnostics retain the actual transport failure message.

The remote provider checks the response identity and serialization version and detaches the deserialized user from its
role/group providers. A user-id-only cache cannot supply this result. The response is a complete computation, not a database
transaction spanning all authorization stores.

PATs and service-only requests do not enter this session operation. Their product, scope, expiry and revocation checks
remain in place; PATs never acquire the expired-JWT exception. Their fresh user read unwraps source caches and uses the
optional `imtbase::IObjectCollectionReadStatus::GetObjectDataWithStatus(objectId, dataPtr, isAccessDenied, dataConfigurationPtr)`
contract when supported, without changing `IObjectCollection`. The per-call flag is initialized to false and becomes true
only for a confirmed access refusal; a refused read returns false. `CRemoteGqlCollectionController` preserves HTTP 401/403
from either the type-id lookup or the object-data request (the single-type shortcut still avoids the type-id request).
A confirmed user-source refusal, including a disabled user's HTTP 403, yields HTTP 403 / WebSocket `AS_DENIED`, with no
selection fallback or JWT session-authorization call. An unavailable or malformed source remains HTTP 500 /
`AS_UNAVAILABLE`; denial while reading a referenced role or group makes the permissions graph incomplete, not partially
usable. Collections without the optional interface retain the existing failure-versus-absence check and cannot report
an explicit refusal separately.
User administration from the product continues through the normal user-authenticated controllers.

Other products and organizations are not answered: `permissions` of another product holds only the permissions stored with the user.

Compatibility: `IsUserComputedBySource` is set in the composition only for an authorization server that hands out the computed user.
An older Puma answers `permissions[P]` only when the set is not empty and otherwise leaves the roles to be expanded by the client; a
product server with the mode set would then see no permissions there. A client wired to `GetSessionAuthorization` requires
the new owner-side controller; a missing operation is an error, not a fallback to cached authorization.
The authorization server is updated before or with its clients. Rebuild the component registries as well as C++ libraries:
the owner's existing authorization handler receives the fresh session source, server-token receiver and permission
repositories; product servers expose the existing remote JWT controller through `PumaGQLClientEngine` and
`AuthorizableServerFramework`.

### The snapshot

Without `IsUserComputedBySource` (Puma itself, Agentino), the user, its roles and its groups are read by
`imtservergql::CAuthorizationSnapshot` at the sources of their collections (`UserCollection`, `RoleCollection`,
`UserGroupCollection` of the authentication manager): a cached collection is passed through by
`IObjectCollectionCacheController::GetSourceCollection`, through every layer. A role or a group is read when the resolution asks for
it, each once. The permissions are resolved by the usual code on what was read, for the products of the user and of its groups, and
kept in the user, which then points to no provider: nothing is read again from a cache, and the user outlives the snapshot. An absent
role or group grants nothing; a role or a group that cannot be read, a missing collection or an object of an unexpected type make the
check unavailable.

This local resolution is not tenant-aware: it is `IUserBaseInfo::GetPermissions`, as for the requests of Puma itself, and it is not
the computation Puma makes for a product server. A role of another organization still grants its permissions there, bounded by the
organization permissions of the context. Making it tenant-aware is a separate change.

The stores tell a failed read from an absent entry: `ITenantManager::GetTenant`, `ITenantMembershipManager::FindMembership`
and `GetMembershipsByUser`, `ITenantInvitationManager::GetInvitationsByUser` report it through `isReadPtr`; a remote organization
read is not answered when the request fails or the server answers with an error, and the session check of the authorization server
answers a session it could not read with an error. A request goes on with what was read but does not cache incomplete organization data.

The snapshot is a set of freshly read data, not an atomic snapshot of several remote collections: the generation guards against the
changes this server is told about, not against every change at the owner of the data.

### When a subscription is checked again

The servlet (`CWebSocketServletComp`) checks the subscriptions with credentials again by `imtservergql::CSubscriptionCheckSchedule`,
on a monotonic clock. The subscriptions with the same credentials (access token, server token, product) are checked as one.

| Reason | When |
|---|---|
| A change of one of its `AccessModels` | At once; the changes of a burst and those during a check give one more check |
| The end of the session (`RenewGqlContext` tells it) | At that time; only an already reached deadline is retried after 10 s. A prolonged session is checked again, not closed |
| Neither | `MaxCheckInterval` (300 s) after the last confirmation |

`AccessModels` lists only the models whose changes may change an access and that a check does not change itself, or a check would
start the next one:

| Server | Models |
|---|---|
| Product server (`AuthorizableServerFramework`) | The caches of the users, roles and groups of `PumaClientEngine`: the pushes of Puma invalidate them |
| Puma (`PumaServerBase`, set by `PumaServerPg`, `PumaServerSl` and their tests) | The repositories of users, roles, groups, memberships, invitations, bindings, cross-organization grants and organizations, and the personal access token manager |

The sessions are not among them: every sign-in and token refresh writes them. `UpdateLastUsedAt` of a token does not notify the
token manager, and reading a collection or filling a cache notifies nobody.
Invitations are included because a pending invitation gives a user outside any organization access to the organization list.
The servlet also observes the local server-token manager exposed by `ServerTokenReceiver`, without requiring it in `AccessModels`.
If that manager is already listed, it is observed only once. The Puma test applications use a 30 s control interval;
the production default remains 300 s. Event-driven test timeouts stay below the test control interval.

A change without an event is found at the control check only, up to `MaxCheckInterval` later: a session closed at Puma, a membership,
an organization or a token changed at Puma seen from a product server, a token changed at another server. This is 300 s instead
of the former 30 s: a subscription is **not** ended at once for such a change.

A check that does not confirm the access (`AS_UNAVAILABLE`, `AS_STALE`) starts the grace period (`GracePeriod`, 60 s) at the start of
its attempt; a check still running counts too, so a check that never answers does not hold a subscription. Only a confirmation ends
it: a change of the access, a stale check or another failed one does not start it again. A stale check is repeated at most once a
second, one that could not be made every 10 s. A subscription whose access is not confirmed within the grace period is closed as
temporarily unavailable. A check runs on a pool of threads with the timeouts of its client: it holds back neither the other checks
nor the end of a grace period, and its answer for a subscription closed meanwhile is not used.
Confirmation at or after the grace deadline cannot revive the subscription, even if its worker finishes before the
scheduler processes that deadline. Queued checks whose subscriptions already ended are discarded before calling the source.

The servlet groups registrations by credential key. A check looks up and visits that key's subscriptions only, rather
than scanning all registrations for every key. Removing the last registration removes its schedule entry immediately;
a late result cannot apply to a subsequent registration of the same credentials.
`CSubscriptionCheckSchedule` keeps timing separate from threads and network calls. Its constructor takes only the
control interval and grace period; the agreed 1 s / 10 s retry limits are implementation constants, not extra settings.

The error frame of a refused or closed subscription carries a machine code in `extensions.code`; the message stays for the clients
that read it:

| Code | Cause | Web client (`SubscriptionManager.qml`) |
|---|---|---|
| `AuthorizationUnavailable` | The grace period is over, or the access could not be checked at the registration | Subscribes again after 5 s, doubled up to 60 s, as long as the answer is the same |
| `PermissionsChanged` | Current access no longer serves the registration, or the recipient changed | Subscribes again at once and reads its state again |
| `Unauthorized` | The token expired or its session is closed | Waits for a refreshed token or a reconnect |
| `Forbidden` | The access is refused | Waits for a refreshed token or a reconnect |

A product server asking Puma by `GetSessionAuthorization` learns the end of the session from `sessionExpiresAt`. An older Puma does not
send it: the end is then unknown, not passed, and the control check comes after `MaxCheckInterval` with every check of the session
authorization unchanged. The ends of personal access tokens and server tokens are not known to the servlet: their expiry is found
by the control check.

## Catalog uniqueness

Product repositories migrate `ProductId` into an indexed physical column and enforce uniqueness of the active
record while retaining historical revisions. Existing conflicting active IDs abort migration without deleting rows.
Repair such conflicts explicitly and restart the component. Concurrent initial registrations re-read the winning row.

## What a receiver is made of

| Part | Component | Role |
|---|---|---|
| The receiver | `ImtAuthVoce/ServerTokenReceiver` | One element of the server registry. It holds everything below and exports `imtservergql::IServerTokenReceiver`: the product of the server and its token store. |
| Token commands | `imtauthgql::CServerTokenControllerComp` | Adds client products, issues, lists, changes and revokes the tokens of this server, offering the permission tree of the server. A superuser session opened at this server only. |
| Allowed clients | `imtauth::IClientProductCatalog` | The client products in the table `ConnectedProducts` of the database of the server. `AddServerTokenClient` adds a product, so a configurator (Agentino) sets up the whole connection. Puma shows the same table as its catalog of products (`ImtAuthVoce/ConnectedProductCatalog` embeds the receiver). |
| Token storage | `ImtAuthVoce/PersonalAccessTokenStore` | The table `PersonalAccessTokens` of the server. A server token starts with `imt_srv_`, a personal token with `imt_pat_`: each is looked up only where its kind is kept. |
| Check of a request | `imtservergql::CAuthenticationManagerComp` | Takes the receiver as its `ServerTokenReceiver` reference. Refuses another product without a token of its server, checks the token, bounds the request by it (above) and checks where a session may be used (below). |

## Making a product a receiver

Add the receiver to the server registry and give it to `AuthorizableServerFramework`:

```xml
<Element Id="ServerTokenReceiver" PackageId="ImtAuthVoce" ComponentId="ServerTokenReceiver">
    <Data IsEnabled="true" Flags="0">
        <AttributeInfoMap>
            <AttributeInfo Id="DatabaseEngine" Type="Reference" ExportId="">
                <Data IsEnabled="true" Value="DatabaseEngine"/>
            </AttributeInfo>
            <AttributeInfo Id="PermissionTree" Type="Reference" ExportId="">
                <Data IsEnabled="true" Value="AuthorizableServerFramework"/>
            </AttributeInfo>
        </AttributeInfoMap>
    </Data>
</Element>
```

`AuthorizableServerFramework`: `ServerTokenReceiver` = `ServerTokenReceiver`, and `ServerTokenReceiver` first in the GraphQL request handlers of the server.
The product ID and the permissions a token may hold come from the permission tree. The client products are kept in PostgreSQL;
with SQLite set `ProductRepository` to an `ImtUserAdministrationVoce/SQLiteConnectedProductRepository` element.

In the settings plugin of the product, list the input connection in `ServerTokenConnectionIds` of its `ConnectionCollection`
(`imtservice::IServerTokenReceiverInfo`). Agentino offers a token only for a connection to such an input.

The calling product reads the token from `IMT_<RECEIVER>_SERVER_CREDENTIAL`
(`imtclientgql::GetServerCredentialVariableName`) once at the start, removes the variable so that the processes it starts
do not inherit it, and sends the token with every request to the receiver.

## Where a session may be used

A session opened through the server of a product (the sign-in, a tenant session, a refresh) names that product in the claim
`productId` of its token and in the table `UserSessions`. It works only through a server, with that server's token, which bounds it:
sent directly it would escape the token. It may pass several servers (ProLife forwards it to Lisa, Lisa to Puma): each is bounded
by its own token. It is refreshed only through the server that opened it. A session opened at the authorization server directly
works only there. A server that receives its own users' sessions (Lisa) accepts the sessions of its product directly.

The commands of the server tokens and of the catalog of products need a superuser session opened at this server directly:
a session that came through a server never manages server tokens.

## Who signs in

The operator signs in at the receiver as its superuser.
A product without its own users (Lisa) forwards the sign-in to Puma, so the superuser of Puma issues its tokens.
The tokens are still issued and kept by the product.

## Known limitations

- A token has no expiration: it would stop a working server. Replace it by issuing a new one.
- The password of the superuser of a product without users of its own (Lisa) passes through the server of that product at the sign-in.
  The binding of sessions keeps the server within its token, but a compromised server can read the password.
- On systems other than Windows a stored token (Agentino) is protected by the file permissions only: there is no protection service of the system.
- A token bounds a request only where a handler checks a permission, a user or the product of the server.
  A command the receiver serves without any authentication stays open to anybody, servers included.
