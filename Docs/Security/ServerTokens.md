# Server tokens between product servers

A product server that calls another product server proves who it is with a server token.
The receiving server issues the token for itself only and keeps it.
The calling server sends the token in the header `x-server-token` with its product in `x-product-id`.

```
Operator (Agentino or the Puma console)        Receiving server (Puma, Lisa, ...)           Calling server (ProLife, ...)
   | Authorization (superuser of the receiver) --> session
   | AddServerTokenClient (clientProductId)    --> the product becomes a client (no change if it is one)
   | GetServerTokenReceiver (clientProductId)  --> permission tree the token can hold
   | CreateServerToken / UpdateServerTokenScopes / RevokeServerToken
   |                                             <-- token value (only at creation)
   | the value reaches the calling server (Agentino passes it in IMT_<RECEIVER>_SERVER_CREDENTIAL)
   |                                                                                            | request
   |                                             <------------------ x-server-token, x-product-id
   |                                             AuthenticationManager: token issued here, for this client
   |                                             the handlers check the request with the permissions of the token
```

## What a token allows

A token holds permissions of the permission tree of the receiver, the same permissions its roles are made of
(Puma: `PumaFeatures.h`, Lisa: `LisaFeatures.h`). There is no list of the commands a server may send:
the handlers of the receiver check a request through a server as they check any request.

- **A server acting on its own** (its token in the authentication header) is a user without a login whose permissions are
  those of its token.
- **A request a server forwards for its user** (the session of the user and `x-server-token`) holds the permissions of the user
  that the token holds too. A superuser forwarded by a server holds the permissions of the token and is no superuser there.
- **What a connected product does for itself and its users needs no permission:** signing its users in, their sessions and
  profiles, their notifications and personal tokens, publishing its permission tree, sending notifications to its own users.
  The handlers limit these to the product of the server and to the user of the request.
- The permissions of a member inside an organization are a mechanism of their own and are not bounded by the token.

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
