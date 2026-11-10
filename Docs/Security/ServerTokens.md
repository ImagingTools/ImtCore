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
   |                                             ProductServerRequestPolicy: command -> required permission
```

## What a receiver is made of

| Part | Component | Role |
|---|---|---|
| The receiver | `ImtAuthVoce/ServerTokenReceiver` | One element of the server registry. It holds everything below and exports `imtservergql::IServerTokenReceiver`: the product of the server, the token manager and what the clients may send. |
| Token commands | `imtauthgql::CServerTokenControllerComp` | Adds client products, issues, lists, changes and revokes the tokens of this server. A superuser session opened at this server only. |
| Allowed clients | `imtauth::IClientProductCatalog` | The client products in the table `ConnectedProducts` of the database of the server. `AddServerTokenClient` adds a product, so a configurator (Agentino) sets up the whole connection. Puma shows the same table as its catalog of products (`ImtAuthVoce/ConnectedProductCatalog` embeds the receiver). |
| Token storage | `ImtAuthVoce/PersonalAccessTokenStore` | The table `PersonalAccessTokens` of the server. A server token starts with `imt_srv_`, a personal token with `imt_pat_`: each is looked up only where its kind is kept. |
| What a client may send | `imtservergql::CServerTokenReceiverComp` | `CommandPermissions`: `Command=Permission`, `Command=` opens it to any connected server. `CollectionPermissions`: `Collection=View[,Edit[,ObjectRead]]`. Only the commands of the generic collection protocol (`GetObjectData`, `GetElementIds`, `OpenDocument`, `SetObjectData`, ...) are resolved by the collection they name; any other command must be declared by name. Anything not declared is refused. |
| Check of a request | `imtservergql::CAuthenticationManagerComp` | Takes the receiver as its `ServerTokenReceiver` reference. Refuses another product without a token of its server, checks the token and the policy, and binds sessions to the server they came through (below). |

## Making a product a receiver

Add one element to the server registry and give it to `AuthorizableServerFramework`:

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
            <AttributeInfo Id="CommandPermissions" Type="Id[]" ExportId="">
                <Data IsEnabled="true">
                    <Values>
                        <Value>GetApplicationInfo=</Value>
                        <Value>LicensesList=/LicenseDefinitionManagement/View</Value>
                    </Values>
                </Data>
            </AttributeInfo>
            <AttributeInfo Id="CollectionPermissions" Type="Id[]" ExportId="">
                <Data IsEnabled="true">
                    <Values>
                        <Value>Products=/ProductManagement/View</Value>
                    </Values>
                </Data>
            </AttributeInfo>
        </AttributeInfoMap>
    </Data>
</Element>
```

`AuthorizableServerFramework`: `ServerTokenReceiver` = `ServerTokenReceiver`, and `ServerTokenReceiver` first in the GraphQL request handlers of the server.
The product ID comes from the permission tree. The client products are kept in PostgreSQL; with SQLite set `ProductRepository`
to an `ImtUserAdministrationVoce/SQLiteConnectedProductRepository` element. Optional attributes: `RequiredPermissions` (every token holds them) and `ProductServerPermissions` (a tree other than the tree of the product).

In the settings plugin of the product, list the input connection in `ServerTokenConnectionIds` of its `ConnectionCollection`
(`imtservice::IServerTokenReceiverInfo`). Agentino offers a token only for a connection to such an input.

The calling product reads the token from `IMT_<RECEIVER>_SERVER_CREDENTIAL`
(`imtclientgql::GetServerCredentialVariableName`) once at the start, removes the variable so that the processes it starts
do not inherit it, and sends the token with every request to the receiver.

## Sessions are bound to the server they came through

A session opened through the server of a product (the sign-in, a tenant session, a refresh) names that product in the claim
`productId` of its token and in the table `UserSessions`. The authorization server accepts the session only through that server,
with its server token; sent directly, it is refused. A session opened at the authorization server directly works only there.
So a server cannot escape the permissions of its token by sending a session without its token,
and a session of the console does not gain the rights of a server. A server that receives its own users' sessions (Lisa)
accepts the sessions of its product directly.

The commands of the server tokens and of the catalog of products need a superuser session opened at this server directly:
a session that came through a server never manages server tokens.

## Who signs in

The operator signs in at the receiver as its superuser.
A product without its own users (Lisa) forwards the sign-in to Puma, so the superuser of Puma issues its tokens.
The tokens are still issued and kept by the product.

## Known limitations

- A token has no expiration: it would stop a working server. Replace it by issuing a new one.
- Without `ProductServerPermissions` a token may be given any permission of the tree of the server, `Edit` ones included.
  Only `ProductServerRequestPolicy` limits what a client really does, so declare there only what the clients need.
- The password of the superuser of a product without users of its own (Lisa) passes through the server of that product at the sign-in.
  The binding of sessions keeps the server within its token, but a compromised server can read the password.
- On systems other than Windows a stored token (Agentino) is protected by the file permissions only: there is no protection service of the system.
- The receiver checks requests that carry a token or a foreign `x-product-id` with a user session.
  A command the receiver serves without any authentication stays open; the policy does not close it.
