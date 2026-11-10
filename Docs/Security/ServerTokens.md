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
Custom persistent registries must wire this interface too. `InMemoryUpdates=true` is an explicit opt-in
only for a private in-memory collection; its read/modify/write operations are serialized by the manager.

Rebuild dependent products and component packages together when deploying these changes.
The C++ context/storage interfaces changed; old binaries must not be mixed with the rebuilt packages.
Rebuild every ImtCore library imported by the products before relinking their executables, including
indirect context consumers such as `imtdeskgql`. Product builds can link prebuilt imported libraries
without rebuilding them; a successful executable link alone does not establish ABI consistency.
Do not rebuild shared static libraries concurrently with product linkers that read them.

Local token-change notifications carry the token ID. Authentication removes the matching cached entry rather than
all PAT entries, and a generation check prevents an in-flight lookup from restoring an invalidated entry.
Unidentified change notifications still invalidate the whole PAT cache. These notifications are process-local;
they are not a distributed cache invalidation protocol.

## WebSocket credential lifetime

A subscription keeps, on the server, the headers it was registered with and the fingerprint of its access
(`imtservergql::GetAuthorizationFingerprint`): the principal, its organization, every permission it holds
(roles, groups, organization permissions, implied permissions) and the bounds of its personal and server tokens.

A check builds the context afresh with `IGqlContextCreator::RenewGqlContext`, without any cache.
Unlike a new request, an expired access token is accepted there while its session lives. A check ends as one of:

| State | Action |
|---|---|
| `AS_CONFIRMED` | Compare the fingerprint; a changed one closes the subscription |
| `AS_DENIED` | Close the subscription |
| `AS_UNAVAILABLE` | Keep the subscription; nothing is known (a failed read is not a removed account or session) |
| `AS_STALE` | The access changed during the check; the result is not used and the check is repeated |

Every change of the access this server is told about increments its authorization generation
(`IGqlContextCreator::GetAuthorizationGeneration`). A registration during which the generation changed holds only
if a fresh check confirms the same fingerprint; otherwise it is rolled back without a success acknowledgement.
A cache fill read before a change is not kept.

The check interval is 30 seconds for now, so revocation is not an instantaneous interruption of an already running callback.

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
