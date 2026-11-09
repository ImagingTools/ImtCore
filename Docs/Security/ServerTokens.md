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
| Token commands | `imtauthgql::CServerTokenControllerComp` | Adds client products, issues, lists, changes and revokes the tokens of this server. A superuser session only. |
| Allowed clients | `imtauth::IClientProductCatalog` | `ConnectedClientProductCatalog`: the client products in a table of the receiver (`ConnectedProductRepository`, the product chooses the one of its database). `AddServerTokenClient` adds a product, so a configurator (Agentino) sets up the whole connection. Puma shows the same table as its catalog of products. |
| Permissions of a token | `ProductServerPermissions` of the controller | A dedicated tree (Puma: `/ProductServer/...`). Without it, the permission tree of the server itself. |
| Token storage | `ImtAuthVoce/PersonalAccessTokenStore` | The `PersonalAccessTokens` table of the server (created automatically). |
| Check of a request | `imtservergql::CAuthenticationManagerComp` | `ServerProductId` refuses another product without a token; `ServerTokenManager` is where the tokens of this server are checked, for requests and for the periodic check of subscriptions: a subscription ends when its token is refused or holds other permissions than when it subscribed. A token is looked up there first in both `x-server-token` and the authentication header: a product sends its own server token in the authentication header for its subscriptions. Either way it stands for its client product and is bounded by the policy. A server token kept anywhere else is refused. |
| What a client may send | `ProductServerRequestPolicy` | Every command or collection a client may call, with the permission it needs; an empty permission opens a command to any connected server. Reading one object of a collection needs its object-read permission or its view permission, opening a document only reads it. Anything not declared is refused. |

`ImtAuthVoce/ServerTokenReceiver` puts the controller, the client catalog and the token store together.

## Making a product a receiver

A product that uses `AuthorizableServerFramework` needs four changes in its server registry.

1. Add the table of the client products and the receiver:

```xml
<Element Id="ClientProductRepository" PackageId="ImtUserAdministrationVoce" ComponentId="ConnectedProductRepository">
    <Data IsEnabled="true" Flags="0">
        <AttributeInfoMap>
            <AttributeInfo Id="DatabaseEngine" Type="Reference" ExportId="">
                <Data IsEnabled="true" Value="DatabaseEngine"/>
            </AttributeInfo>
        </AttributeInfoMap>
    </Data>
</Element>
<Element Id="ServerTokenReceiver" PackageId="ImtAuthVoce" ComponentId="ServerTokenReceiver">
    <Data IsEnabled="true" Flags="0">
        <AttributeInfoMap>
            <AttributeInfo Id="DatabaseEngine" Type="Reference" ExportId="">
                <Data IsEnabled="true" Value="DatabaseEngine"/>
            </AttributeInfo>
            <AttributeInfo Id="ProductRepository" Type="Reference" ExportId="">
                <Data IsEnabled="true" Value="ClientProductRepository"/>
            </AttributeInfo>
            <AttributeInfo Id="PermissionTree" Type="Reference" ExportId="">
                <Data IsEnabled="true" Value="AuthorizableServerFramework"/>
            </AttributeInfo>
        </AttributeInfoMap>
    </Data>
</Element>
```

   With SQLite the table is `SQLiteConnectedProductRepository`. Optional attributes: `RequiredPermissions` (every token holds them) and `ProductServerPermissions` (a tree other than the tree of the product).

2. Let `AuthorizableServerFramework` check the tokens there and name the server:
   `ServerTokenManager` = `ServerTokenReceiver/ServerTokenManager`, `ServerProductId` = the product ID.
3. Add `ServerTokenReceiver` to the GraphQL request handlers of the server, before `AuthorizableServerFramework`.
4. Declare in `ProductServerRequestPolicy` every command and collection the clients call, with the permission each needs.
   The permissions are the IDs of the tree, for example `/ProductManagement/View`.

The calling product reads the token from `IMT_<RECEIVER>_SERVER_CREDENTIAL`
(`imtclientgql::GetServerCredentialVariableName`) and sends it with every request to the receiver.

## Who signs in

The operator signs in at the receiver as its superuser.
A product without its own users (Lisa) forwards the sign-in to Puma, so the superuser of Puma issues its tokens.
The tokens are still issued and kept by the product.

## Known limitations

- A token has no expiration: it would stop a working server. Replace it by issuing a new one.
- Without `ProductServerPermissions` a token may be given any permission of the tree of the server, `Edit` ones included.
  Only `ProductServerRequestPolicy` limits what a client really does, so declare there only what the clients need.
- The receiver checks requests that carry a token or a foreign `x-product-id` with a user session.
  A command the receiver serves without any authentication stays open; the policy does not close it.
