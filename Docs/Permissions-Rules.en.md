# Permission Organization Rules for ImtCore- and Lisa-Based Solutions

> **Language:** English · [Русский](Permissions-Rules.ru.md) · [Deutsch](Permissions-Rules.de.md)
>
> **Purpose.** A unified set of rules for describing, assigning, checking, and testing permissions in products based on ImtCore and Lisa.
> **Sources.** `ItDevelopment` code: ImtCore `442be502b9` (2026-10-15), Lisa `a598f56` (PR #28, “Full-permissions”), ProLife `d4d9a910`, Puma `e8b53c6`; Partitura (`.acc`) files, product `*Features.h` files, QML, and tests. Paths below are relative to `ItDevelopment/`.
> **Implementation status.** Branches `feature/permission-requirements` (ImtCore), `feature/organization-permissions-by-path` (Puma), `feature/permission-model-tests` (ProLife), and `feature/permission-paths` (Lisa), as of 2026-10-19: path-only permissions, new Lisa/ProLife and organization trees, and revisions scoped to each entity.
> **How to read this document.** Sections 3–12 describe the rules and **target** behavior. Where the code still behaves differently, see section 13. Section 13 lists code discrepancies and open questions.

---

## 1. Component responsibilities

| Component | Responsibility for permissions |
|---|---|
| **Lisa** | Product, feature, and license catalog. The product **feature tree** is edited here (`FeatureEditor`) and generates `<Product>Features.h` (`ImtCore/Include/imtlicgql/CProductInfoFileGeneratorComp`). Lisa is also a product (`Lisa`) with its own tree. |
| **ImtCore** | Runtime: model (`imtlic`, `imtauth`), checking (`imtservergql`, `imtserverapp`, `imtauthgql`), GUI (`imtauthgui`, `imtlicgui`), and ready-to-use Partitura components (`ImtCoreVoce.arp`, `ImtAuthVoce.arp`, `ImtUserAdministrationVoce.arp`). |
| **Puma** | Account service (users, roles, groups, organizations). Product servers obtain the user and permissions through it (`AdminId = su`). |
| **Product** (Lisa, ProLife, NeoPro, Acula, Agentino, IotPlatform) | Describes its feature tree and declares server-command and GUI-page permissions in its `.acc` files. |

## 2. End-to-end flow

```
Lisa FeatureEditor ──generate──► <Product>Features.h ──FillProduct()──► IProductInfo (product feature tree)
                                                                                  │
Roles / groups / local permissions (leaf nodes only) ──► Puma                    │
         product permissions, filtered by organization                           ▼
                                                CAuthenticationManagerComp: creates IGqlContext
                                                (user, tenant, product, PAT scopes, IsTenantOwner)
                                                                                  │
                                                effective permissions = (permissions ∩ PAT scopes)
                                                + implied requirements, ∩ organization permissions
                                                                                  │
                ┌─────────────────────────────────────────────────────────────────┤
                ▼                                                                 ▼
      Server commands (GraphQL)                                     GUI: pages and commands
      CPermissibleGqlRequestHandlerComp                              CCommandsController / PagesController
      ICommandPermissionsProvider + IPermissionChecker              return only accessible elements
                │                                                                 │
                └──────────────── imtauth::HasPermission ◄───────────────────────┘
                                  (one shared comparison rule)
```

**Principle:** the server is the security boundary. The GUI (hiding pages and buttons) is for convenience, not protection.

## 3. Describing permissions (feature tree)

**R3.1. The product feature tree in Lisa is the single source of truth.** `<Product>Features.h` is generated and must not be edited by hand. At startup, the server calls `FillProduct(*IProductInfo)` (`Lisa/Impl/LisaServer/Main.cpp`); for GraphQL requests, `ProductPermissions` reads the tree from the `:/Features/<Product>Features` resource (`SingleProductPermission` component). Lisa imports `<Product>Features.h` as well as XML: the file creates a new product; a product with the same `ProductId` is not imported, nor is a root already present in the feature catalog under another UUID. Features from the file are written to the catalog: new ones are added and existing ones are replaced by the versions in the file (the import dialog warns about this).

**R3.2. Permission node:** `SetIsPermission(true)`, `SetOptional(false)`. A permission grants an action in the application; it is not a feature sold to customers. Marketable features (`IsPermission=false`, `Optional=true`) are not part of the permission tree. All nodes in all current products are permissions.

**R3.3. Node identifier (`FeatureId`):** Latin characters, PascalCase, no `/`, unique among siblings. The entity names the area, so identifiers within it are short (`View`, `Edit`, `Add`) and are reused in other areas—the **full path alone** is a permission (section 4). Paths are stored in the roles and users database; renaming a node requires a migration. Display names use `QT_TRANSLATE_NOOP("Feature", "...")`.

**R3.4. Typical area structure** (Lisa and ProLife, modeled on the organization tree in R3.6):

```
/<Area>Management                     area group (FeatureManagement, SensorManagement, ...)
├─ ViewPage                           leaf: area page (menu, page)
├─ View                               leaf: data (collection, documents, commands)
├─ ViewAll, ViewHistory → View        leaves, where present (ProLife)
├─ Edit → View                        group: any modification
│  ├─ Add                             leaf: creation
│  ├─ Change                          leaf or group of fields: Change/{Description, SerialNumber, ...}
│  └─ Remove                          leaf: deletion
└─ Revisions                          revisions for documents in this area
   ├─ View → <Area>/View
   ├─ Restore → Revisions/View
   └─ Delete → Revisions/View
```

**`ViewPage` and `View` are independent** and have no requirements between them: area data can be displayed without its page (a sensor table in another editor, a selection list, an API), while a page without `View` opens empty. A page requires `ViewPage`; data requires `View`. A role must grant both when both are needed.

Area-specific actions are leaves alongside `View` (`/SensorManagement/Bind`, `/SensorManagement/CreateLicenseFile`) or inside `Edit` (`/LicenseManagement/Edit/Split`, `/LicenseManagement/Edit/Revoke`). Administration: `/Administration/{Users,Roles,Groups}/{ViewPage, View, ViewHistory → View, Edit → View/{Add, Change, Remove}, Revisions}`. Workspace: `/WorkspaceManagement/{ViewPage, ViewUserActions, ViewAllUserActions → ViewUserActions}` (ProLife also has `ViewAnalytics`). Search: `/SearchManagement/ViewPage`—these areas have no collection of their own. There is no longer a cross-cutting revision group (R6.7).

**R3.5. Users and roles store leaves only** (as full paths). A group only organizes leaves; it cannot be granted. Requirements on a group apply to its leaves, not the group itself (section 5). Group paths are used only in **checks** (R4.4).

**R3.6. The organization tree is separate.** Permissions within an organization are defined by `imtauth::COrganizationsPermissionsProviderComp`, not by the product. Do not mix it with the product tree. It has one root and areas following the R3.4 pattern:

```
/OrganizationManagement
├─ Edit/{ChangeName, ChangeDescription}
├─ Members/{View, Edit → Members/View /{Invite, Exclude, ChangeRole}}
├─ Roles/{View, Edit → Roles/View /{Add, Change, Remove}}
├─ Groups/{View, Edit → Groups/View /{Add, Change, Remove}}
├─ Permissions/{View, Change → Permissions/View}
└─ Connections/{View, ViewCode, Edit → Connections/View /{Connect, Remove}}
```

Permissions in this tree, like product permissions, are stored and checked only as full paths (section 4).

**Two `/OrganizationManagement` trees serve different purposes.**
- **Organization tree** (above)—static member permissions: what a user may do **within** the organization. They are stored in the membership (`ITenantMembership::GetOrganizationPermissions`), checked by the server in the commands themselves (`imtauthgql::HasTenantManageAccess`, `CanManageMembers`), and returned to the client as `currentUserOrganizationPermissions`. Owners, creators, and superusers pass without checking a leaf.
- **Product tree** (ProLife) is only for visibility of the page, data, and commands:

```
/OrganizationManagement
├─ ViewPage        “Organizations” page
├─ View            organization data and all commands (read, edit, members, connections, ...)
├─ Add             create an organization
├─ Remove          delete an organization
└─ Revisions/{View → View, Restore, Delete}
```

Internal organization commands are declared with product permission `/OrganizationManagement/View`; the member leaf determines which action is allowed:

| Commands | Member permission leaf |
|---|---|
| Update organization (`UpdateTenant`) | `Edit/ChangeName` or `Edit/ChangeDescription` |
| `SetTenantActive`, `RemoveTenant` | Owners, creators, and superusers only |
| Read members and invitations | `Members/View` (own membership requires no permission) |
| Invite, exclude, or change the role of a member | Any `Members/Edit/*` leaf (13.23) |
| `GetTenantPermissions` | `Permissions/View` or `Roles/View` |
| Read another user's membership permissions (`GetOrganizationPermissions`) | `Permissions/View` |
| Change a member's permissions (`UpdateMembershipPermissions`) | Any `Members/Edit/*` leaf (13.23) |
| Read connections, requests, offers, contracts, grants, messages, and applications | `Connections/View` |
| Connection code | `Connections/ViewCode` |
| Create, accept, reject, or cancel; edit contracts, grants, messages, or applications; change the connection code | `Connections/Edit/Connect` |
| Remove a connection | `Connections/Edit/Remove` |

## 4. Identifier format and comparison rule

**The only format is the full node path:** `/Administration/Users/Edit/Change` (`imtlic::CalculateFeaturePath`), always with a leading `/`. A bare `FeatureId` (`ChangeUser`) is **not** a permission: it grants nothing to a user, matches nothing in a check, and does not expand requirements. Stored bare identifiers must be migrated to paths (13.5).

**R4.1. One shared rule: `imtauth::HasPermission(user permissions, requested permission)`** (`ImtCore/Include/imtauth/imtauth.h`); on the client, `PermissionsController.hasPermission` (QML, see 13.3). Every permission check must use this function; `contains`/`indexOf` on a permission list is not allowed.

**Tree used in examples**—a fictional tree, matching the `PermissionRequirementsTest` unit tests (product trees follow R3.4; long identifiers make examples self-explanatory):

```
/Administration
├─ ViewAdministration          group
│  ├─ ViewUsers                leaf
│  └─ ViewRoles                leaf
└─ EditAdministration          group
   ├─ EditUser                 group
   │  ├─ ChangeUser            leaf
   │  └─ RemoveUser            leaf
   └─ EditRole                 group
      └─ ChangeRole            leaf
/RevisionManagement            group
├─ ViewRevisions               leaf
└─ ExportRevision              leaf
/Reports/ViewUsers             leaf (same identifier under a different parent)
```

**Comparison rule**

| User holds (leaf) | Check | Result |
|---|---|---|
| `/Administration/EditAdministration/EditUser/ChangeUser` | same full path | **yes**—same leaf |
| `/Administration/EditAdministration/EditUser/ChangeUser` | `/Administration/EditAdministration/EditUser` | **yes**—a group check means “at least one leaf inside” |
| `/Administration/EditAdministration/EditUser/ChangeUser` | `/Administration` | **yes**—the same rule at any depth |
| `/Administration/EditAdministration/EditUser/ChangeUser` | `/Administration/EditAdministration/EditUser/RemoveUser` | no—sibling leaf |
| `/Administration/ViewAdministration/ViewUsers` | `/Administration/EditAdministration` | no—the user holds no leaves in this group |
| `/Administration/ViewAdministration/ViewUsers` | `/Reports/ViewUsers` | no—the same identifier under a different parent is a different permission |
| `ChangeUser` (bare identifier, old data) | `/Administration/EditAdministration/EditUser/ChangeUser` | no—a bare identifier is not a permission |
| `/Administration/EditAdministration/EditUser/ChangeUser` | `ChangeUser` (bare identifier) | no—a bare-identifier check always fails |
| `/Administration/EditAdministration/EditUser` (group stored as a permission) | `/Administration/EditAdministration/EditUser/ChangeUser` | no—a group cannot be granted (R3.5) and does not cover a leaf |
| any | empty string | no |

**R4.2.** Path boundaries are `/`: `/Administration/EditAdministration/EditUser` covers `.../EditUser/ChangeUser`, but not `.../EditUsers` (a different node with a similar name).
**R4.3.** Full paths are required everywhere: `.acc`, `Features.h`, C++, QML, requirements, PAT scopes, and role/user permissions. A shared ImtCore component that cannot know a product path in advance receives it from the host through a property (`revisionsPermissionPath`, `permissionPath`) instead of checking a bare identifier. A bare-identifier declaration opens a command to nobody except the superuser and organization owner (13.21).
**R4.4.** A group path is allowed in a **check** (for example, `PagePermissions` contains `/FeatureManagement/EditFeature`) and means “the user has at least one leaf inside.” A group cannot be granted (R3.5).

## 5. Requirements—what a permission includes

**Meaning.** A node's `requirements` are the permissions it **includes**. `ChangeUser` → `ViewUsers` means that anyone with `ChangeUser` also has `ViewUsers`. The reverse does not apply: `ViewUsers` does not grant `ChangeUser`. Implied permissions are not stored in a role; they are calculated during checks.

**R5.1. Four dependency types.** The source may be a leaf or a group; the target may be a leaf or a group. User permissions always contain leaves only (R3.5).

| Dependency | Example | Meaning |
|---|---|---|
| leaf → leaf | `ChangeUser` → `ViewUsers` | Anyone with `ChangeUser` also has `ViewUsers` |
| group → leaf | group `EditUser` → `ViewUsers` | Any `EditUser` leaf (`ChangeUser`, `RemoveUser`, …) includes `ViewUsers` |
| leaf → group | `ChangeUser` → group `ViewAdministration` | `ChangeUser` includes **all** leaves in `ViewAdministration` |
| group → group | group `EditAdministration` → group `ViewAdministration` | Any `EditAdministration` leaf includes all `ViewAdministration` leaves |

**R5.2. Calculation**—on demand for every permission check, using the product tree (`IProductInfo`) from the request context:
1. Start with the user's product leaves after organization filtering; for a PAT, intersect them with the token scopes (section 10).
2. Each leaf includes its own requirements and the requirements of every group containing it.
3. A leaf target adds that leaf; a group target adds all leaves in the group at check time.
4. Expand the resulting leaves in the same way while the set grows: transitively, including requirements of their groups. Cycles are allowed.
5. Limit the result to organization permissions (`GetTenantPermissions()`).
6. These are the **effective permissions**. Check with `HasPermission(effective permissions, requested permission)` (section 4).

Without `IProductInfo` in the context, requirements are not expanded and the user has only the granted leaves. The presence of `FeatureContainer` in `AuthenticationManager` is checked at server startup.

**R5.3. Record dependencies from “stronger permission → what it includes”:** `Edit*` → `View*`, never the reverse. A single requirement on a group is usually best: `EditUser` → `ViewUsers` applies to `ChangeUser`, `RemoveUser`, and `AddUser`. Lisa, ProLife, and the organization tree are configured this way; NeoPro, IotPlatform, and Acula still use the reverse direction (13.1).

**R5.4. Constraints** (enforced by `FeatureEditor` and the `Features.h` generator):
- The target must be an existing node in the same product, specified by full path; `LicenseEditor` marks external requirements as “from outside this product.”
- A node cannot depend on **itself, its ancestors, or its descendants**.

**R5.5. Group targets are expanded at check time.** A newly added leaf in a target group is immediately granted to everyone whose dependency points to that group, without changing their roles. `FeatureEditor` therefore shows each node's effective inclusions (for example, “`ChangeUser` includes: `ViewUsers`, `ViewRoles`”) and warns when a leaf is added to a group that is a dependency target.

**R5.6. The server returns effective permissions to the client** (granted + implied); otherwise buttons made available through implied permissions would be hidden. The role editor shows implied permissions separately and disabled: `ViewUsers` cannot be removed while the role has `ChangeUser`.

### Examples

The tree is from section 4. Names in the tables abbreviate full paths: `ChangeUser` = `/Administration/EditAdministration/EditUser/ChangeUser`, group `EditUser` = `/Administration/EditAdministration/EditUser`, and so on. `→` means “includes.”

**Leaf → leaf**

| User holds | Requirements | Check | Result |
|---|---|---|---|
| `ChangeUser` | `ChangeUser` → `ViewUsers` | `ChangeUser` | **yes**—granted |
| `ChangeUser` | `ChangeUser` → `ViewUsers` | `ViewUsers` | **yes**—included by `ChangeUser` |
| `ChangeUser` | `ChangeUser` → `ViewUsers` | `ViewRoles` | no—only `ViewUsers` is included |
| `ViewUsers` | `ChangeUser` → `ViewUsers` | `ChangeUser` | no—View does not grant Edit |
| `ChangeUser` | `ChangeUser` → `ViewUsers` | group `ViewAdministration` | **yes**—included `ViewUsers` is in the group |
| `ChangeUser` | `ChangeUser` → `ViewUsers`, `ViewUsers` → `ViewRevisions` | `ViewRevisions` | **yes**—inclusion is transitive |

**Group → leaf**

| User holds | Requirements | Check | Result |
|---|---|---|---|
| `RemoveUser` | group `EditUser` → `ViewUsers` | `ViewUsers` | **yes**—the group dependency applies to its leaf |
| `ChangeRole` | group `EditUser` → `ViewUsers` | `ViewUsers` | no—`ChangeRole` is not in `EditUser` |

**Leaf → group**

| User holds | Requirements | Check | Result |
|---|---|---|---|
| `ChangeUser` | `ChangeUser` → group `ViewAdministration` | `ViewRoles` | **yes**—all group leaves are included |
| `ChangeUser` | `ChangeUser` → group `ViewAdministration` | `/Reports/ViewUsers` | no—the same leaf identifier outside the group is not included |
| `ViewRoles` | `ChangeUser` → group `ViewAdministration` | `ChangeUser` | no—the dependency does not work in reverse |

**Group → group**

| User holds | Requirements | Check | Result |
|---|---|---|---|
| `ChangeRole` | group `EditAdministration` → group `ViewAdministration` | `ViewUsers` | **yes**—any Edit-branch leaf includes all View-branch leaves |
| `ViewUsers` | group `EditAdministration` → group `ViewAdministration` | `ChangeRole` | no—the dependency does not work in reverse |
| `ChangeUser` | group `EditAdministration` → group `ViewAdministration`; group `ViewAdministration` → `ViewRevisions` | `ViewRevisions` | **yes**—included leaves bring their group's requirements |
| `ChangeUser` | same | `ExportRevision` | no—only `ViewRevisions` is included |

**Boundaries and special cases**

| User holds | Requirements | Check | Result |
|---|---|---|---|
| `ChangeUser`; PAT scopes `[ChangeUser]` | `ChangeUser` → `ViewUsers` | `ViewUsers` | **yes**—requirements expand from token scopes |
| `ChangeUser`, `RemoveUser`; PAT scopes `[ChangeUser]` | `ChangeUser` → `ViewUsers` | `RemoveUser` | no—outside the token scopes |
| `ChangeUser`; organization allows `ChangeUser`, not `ViewUsers` | `ChangeUser` → `ViewUsers` | `ViewUsers` | no—organization boundary. The organization permission editor will not save this set: it adds implied leaves itself (section 10) |
| `ChangeUser`; a new `ViewGroups` leaf is added to `ViewAdministration` | `ChangeUser` → group `ViewAdministration` | `ViewGroups` | **yes**—immediately after the tree update, without changing roles (R5.5) |
| `ChangeUser` (bare identifier, old data) | `ChangeUser` → `ViewUsers` | `ViewUsers` | no—a bare identifier is not a permission and includes nothing; old data is migrated separately (13.5) |
| `ChangeUser`; no `IProductInfo` in context | `ChangeUser` → `ViewUsers` | `ViewUsers` | no—requirements are not expanded; this is a configuration error (R5.2) |

**Forbidden dependencies** (rejected by the generator and `FeatureEditor`, R5.4)

| Dependency | Why it is forbidden |
|---|---|
| `ChangeUser` → `ChangeUser` | self-dependency |
| `ChangeUser` → group `EditUser` | dependency on its parent: `ChangeUser` would get `RemoveUser` and `AddUser` |
| `ChangeUser` → group `/Administration` | dependency on an ancestor: it would grant the entire area, including `ChangeRole` |
| group `EditUser` → `ChangeUser` | dependency on its descendant |
| group `EditAdministration` → group `EditUser` | dependency on a descendant: `ChangeRole` would get `RemoveUser` |

## 6. Assigning permissions to server commands

### 6.1. Components (Partitura, `ImtServerAppPck` package)

| Component | Purpose |
|---|---|
| `SingleCommandPermissionsProvider` | One `CommandId` and a list of `CommandPermissions`. |
| `CommandPermissionsProviderJoiner` | Combines providers into one (`CommandPermissionsProvider[]`); when `CommandId` conflicts, the **first** provider wins (13.9). |
| `ObjectCollectionPermissions` (`ImtAuthVoce`) | Standard mapping of collection commands (`GetObjectData`, `InsertNewObject`, …) to permissions. |
| `CPermissibleGqlRequestHandlerComp` and subclasses | Handler with `CommandPermissions` (provider) and `PermissionChecker` attributes. |
| `ImtAuthPck/CheckPermissions` / `CheckPermissionOr` | Checker for **all** (AND) / **any** (OR). |

### 6.2. Check algorithm (`imtservergql::CheckRequestPermissions`)

1. No provider connected → **allow** (13.10).
2. No context or user → **deny**.
3. Superuser (`IsAdmin`) or organization owner (`IsTenantOwner`) → **allow** (for PAT, see 13.16).
4. Command is not in the provider → **deny** (`denyUndeclared = true`).
5. No checker connected or command permission list is empty → **allow**.
6. The request has a `permissionPath` header (6.4) → the path must **match one of the command permissions or be inside one**, and the user must hold it among their effective permissions.
7. Otherwise, the checker compares the user's **effective** permissions (section 5) with the declared list.

### 6.3. Rules

- **R6.1.** Declare every command handled by a `CPermissibleGqlRequestHandlerComp` subclass in a provider. Undeclared commands are denied.
- **R6.2.** Always connect **both** attributes (`CommandPermissions` and `PermissionChecker`). If either is missing, the handler is open to everyone (fail-open; see `ImtCore/Docs/GQL_CONTEXT_THREAD_PROPAGATION_BUG.md`).
- **R6.3.** Public commands (login, token, session refresh, registration) are not permissible handlers.
- **R6.4.** “Action → permission” convention (R3.4): read → `<Area>/View`; create → `<Area>/Edit/Add`; modify, rename, describe → `<Area>/Edit/Change` (or a field leaf `Edit/Change/<Field>`); delete → `<Area>/Edit/Remove`. See Lisa `ProductPermissions.acc`. Export → `<Area>/View`, import → `<Area>/Edit/Add`, restore a deleted object → `<Area>/Edit/Remove` (13.21).
- **R6.5.** A server command usually has **one** permission; multiple permissions mean AND (`CheckPermissions`). GUI elements (pages, buttons) use an **OR** checker (`Command.acc` and `Page.acc` use `CheckPermissionOr`).
- **R6.6.** For standard collections, use `ObjectCollectionPermissions`: `GetObjectData/Headers/ElementIds/MetaInfo/…` → `<Area>/View`; `InsertNewObject` → `<Area>/Edit/Add`; `SetObjectData/Name/Description` → `<Area>/Edit/Change`; `RemoveElements(Set)` → `<Area>/Edit/Remove`.
- **R6.7. Revisions are a `Revisions` subgroup in each area** (R3.4). Revision controllers (`CDocumentRevisionControllerComp`, remote `CRemoteDocumentRevisionControllerComp`) receive `RevisionPermissionPaths`, one path for each `CollectionIds` entry, in the same order (`Devices` → `/SensorManagement/Revisions`). A command requires the leaf for its action in the collection's area: `GetRevisionInfoList` → `Revisions/View`, `RestoreRevision` → `Revisions/Restore`, `DeleteRevision` → `Revisions/Delete` (`imtservergql::CheckRevisionPermissions`). A collection without a path and every other command (`BackupRevision`, `ExportRevision` stubs) are denied. Revisions for one entity do not grant access to another entity's revisions. Client history components (`DocumentHistoryView`, `DocumentRevisionDialog`, collection delegates) receive the area path from the host through `revisionsPermissionPath`.
- **R6.8.** Do not use `*` in permission declarations: the OR checker allows every user when it is present (13.15).

### 6.4. `permissionPath`

A reusable component (ticket panel, chat, `ClientStyle`) may be embedded in different places. The host passes it `permissionPath` (QML property → HTTP `permissionPath` header); the path always begins with `/` (one is added if absent). Use it when one command serves several contexts with different permissions.

Example: command `TicketsList` is declared with `/Support/ViewTickets` and `/Devices/ViewDeviceTickets`.

| `permissionPath` header | User's effective permissions | Result |
|---|---|---|
| `/Devices/ViewDeviceTickets` | `/Devices/ViewDeviceTickets` | **yes**—declared for the command and held by the user |
| `/Devices/ViewDeviceTickets` | `/Support/ViewTickets` | no—the command is running in the device context; the support permission does not apply |
| `/Devices` | `/Devices/ViewDeviceTickets` | no—the path is broader than the declared permission; otherwise any `/Devices` leaf would match (13.12) |

## 7. GUI pages and commands

- **Pages.** `PagePermissions` = `['<Area>/ViewPage']`—the page leaf only (R3.4). Collection commands and data, including `GetCommands`, remain on `<Area>/View`: a collection may appear without its page. The Administration page is opened by any of `/Administration/{Users,Roles,Groups}/ViewPage`; its nested pages use their own `ViewPage` (`AdministrationView.qml`). See `Lisa/Partitura/LisaQmlVoce.arp/Pages.acc`.
- **Collection and document commands.** `NewPermissions`, `EditPermissions`, `SavePermissions`, `UndoPermissions`, `RedoPermissions`, `ClosePermissions` attributes in `StandardCollectionCommands` / `StandardDocumentCommands`. Example (Lisa, Features): `New` → `/FeatureManagement/Edit/Add`; `Edit` → `Edit/Change | Edit/Remove | View`; `Save/Undo/Redo` → `Edit/Add | Edit/Remove | Edit/Change`; `Close` → any permission, including `View`.
- **Server-side filtering.** `CCommandsControllerComp` and the page controller return only elements accessible with **effective** permissions. This is required for **menus, pages, and command panels**.
- **Local QML checks.** `PermissionsController.checkPermission(id)` (`imtauthgui` singleton): superuser (`login === "su"`) → `true`; otherwise `hasPermission(permissions, path)` checks effective permissions received from the server (R5.6), using the same server rule (section 4): full paths only, and a group check means “at least one leaf inside.”
- **R7.1.** GUI checks duplicate, but do not replace, server checks. Every action available through a button must be protected by a server command.
- **R7.2.** One component has one stable permission set. Do not encode permissions in QML to bypass `.acc`; pages and commands obtain them from Partitura.

## 8. Where user permissions come from

```
permissions(product, tenant) = user's roles in the product
                                (local role permissions + parent roles, transitively)
                             ∪ user's groups (parent groups, group roles)
                             ∪ user's local permissions
    → only roles/groups/permissions assigned to the current organization
    → (for PAT) ∩ token scopes
    → + permissions implied by requirements (section 5), ∩ organization permissions
    = effective permissions
```

- **R8.1. Permissions are product-specific** (`IRole::GetProductId`, `IUserBaseInfo::GetRoles(productId)`, `GetPermissions(productId)`). The server determines the request product from its configuration, not the client (13.13). Roles in another product do not grant access to this product's commands.
- **R8.2. Organization (tenant).** For an organization, only roles, groups, and permissions **assigned** to it are considered; for “no organization,” only unassigned (global) ones apply. A parent role/group from another organization grants nothing. A delegated role (cross-org delegation) counts only when its product matches (`imtauthgql::AdaptUserForTenant`, `GetEffectiveUserPermissions`, `GetTenantUserPermissions`). This also applies when checking commands, including in Puma itself (13.14).
- **R8.3. Role and group cycles are forbidden:** a parent role/group that closes a cycle is rejected with an error (`CreatesRoleCycle`, `CreatesGroupCycle`).
- **R8.4. Prohibitions** are stored but **not applied** when calculating permissions (13.8). Do not use them.
- **R8.5.** For remote consumers (ProLife, etc.), role-derived permissions are **materialized** in a serializable copy of the user (`CUserCollectionControllerComp::CreateAdaptedObjectData`); otherwise, a PAT user without roles has no permissions.
- **R8.6.** Users can see their assigned roles and groups regardless of permissions: reading their **own** roles (including delegated roles) and groups does not require a check. Other users' roles and groups require permissions (groups currently differ; see 13.7).

## 9. Privileged principals

| Principal | How it is identified | Behavior |
|---|---|---|
| **Superuser** | `CUserInfoComp::IsAdmin()`: `UserId == AdminId` (`AdminId = su` in compositions). The client checks `login == "su"`. | Bypasses all command permission checks; the server returns an **empty** permission list. |
| **Organization owner** | `ITenantInfo::GetOwnerId() == userId` for the current organization (cached on the token). | Bypasses checks within their organization. |

**R9.1.** Do not identify a superuser from the permission list: it is empty and indistinguishable from a user with no permissions. Tests use `permissions: ['*']` for this (`GuiTestKit/fixtures/defineUsers.js` refuses `su` without it).
**R9.2.** Superuser-only actions: enable/disable an account (`IsSuperuserRequest`), change another user's profile (`OnSetProfile`), create a disabled account, and issue PATs with any scopes.
**R9.3.** A disabled account gets no context (`CCS_FORBIDDEN`); its tokens and sessions stop working immediately.
**R9.4.** Privileged-principal bypasses do not remove token limits: a PAT with narrow scopes remains narrow for both superusers and organization owners (13.16).

## 10. Organizations and PATs

**Organizations (tenants).**
- `ITenantInfo::GetTenantPermissions()` is the **upper bound**: the product permissions available to the organization.
- An organization role may contain only permissions from this list (`CRoleDocumentValidatorComp`: “Permission '%1' is not allowed for tenant”).
- Organization permissions are consistent with requirements: when a permission is added, the editor also adds the leaves it includes. Otherwise, `ChangeUser` without `ViewUsers` would allow modification without viewing.
- `GetProductPermissions(productId, tenantId)` returns the tree limited by organization permissions; `GetUserPermissions(productId)` returns the current user's permissions (the superuser receives the whole tree).
- “Global” permissions in the authorization response are the user's permissions that are not part of any organization they belong to (`CalculateGlobalPermissions`).
- Organization management uses a separate tree (R3.6); the owner/creator is considered an organization administrator.

**Personal Access Tokens.**
- Token scopes are a subset of the **creator's** permissions: `HasPermission(creator's effective permissions, scope)`. A superuser can issue any scopes; a limited token cannot create a broader one.
- A scope is a permission or group path (`/OrderManagement/View`, `/OrderManagement`); a bare identifier scope (`ViewOrders`) covers nothing.
- PAT permissions = `user permissions ∩ scopes` using the same `HasPermission` rule (13.4). Requirements are expanded **from this intersection**, not from all user permissions: a token scoped to `ChangeUser` can view users but nothing beyond that (section 5). Roles and groups are cleared; **an empty scope list grants no permissions** (fail-closed).

## 11. Testing

- **Unit tests:** `ImtCore/Tests/PermissionRequirementsTest` (25) covers `HasPermission` (section 4 table, bare identifiers) and requirement expansion (section 5 tables: all four dependency types, transitivity, cycles, organization boundary, request product, and forbidden dependencies). `ImtCore/Tests/PermissionCheckTest` (38) tests complete server-command checks (`CheckRequestPermissions`): principals, declarations, all requirement types, product and organization, `permissionPath`, GUI elements, and revisions (`CheckRevisionPermissions`). New cases were mutation-tested: each fails if bare-identifier comparisons or revision checks by group/another collection are restored.
- **API (Postman):** Puma `TenantApiPostman`, folder 20 “Organization permissions by path” (path-based organization permissions; bare identifiers and groups grant nothing); ProLife `ProLifeApiPostman`, folder 11 “Permission Model” (server product, PAT path and bare-id scopes, and `GetPermissions` matching server decisions).
- **Server API tests:** for each protected command, a user without permission is denied; for each requirement, a permission includes its target but the target does not grant the source.
- **GUI (`GuiTestKit`).** Do **not** maintain permission-to-page matrices: the client is built from the user's permissions, so tests ask the client itself—`page.isAvailable()`, `page.commands.isAvailable(id)`, `admin.hasSubPage(id)`. Skip only with `requires(user, permission)` (not `!user.can(x)`: “could not determine” is not “does not have permission”).
- **User fixtures** list leaves **as named by the server** (`GetProductPermissions(productId)`). `seed.js` checks them against the live tree, and an obsolete identifier fails the run. Fixtures do not list implied permissions; the server adds them. Do not derive the list from `.acc`/QML: `EditFeature` in `PagePermissions` is a group, not a grantable permission.

## 12. Checklist

**Add a permission:** (1) create a leaf in Lisa `FeatureEditor` with `IsPermission=true`, following R3.2–R3.5; (2) add requirements if needed, following R5.3–R5.4 (usually group `<Area>/Edit` → leaf `<Area>/View`); (3) regenerate `<Product>Features.h`; (4) add localization (`Feature`); (5) update `FULL_ACCESS` in GUI test fixtures.

**Protect a command:** (1) `SingleCommandPermissionsProvider` (`CommandId` = GraphQL command id, `CommandPermissions` = full leaf path); (2) add it to `CommandPermissionsProviderJoiner`; (3) handler inherits `CPermissibleGqlRequestHandlerComp` and connects `CommandPermissions` and `PermissionChecker`; (4) use `permissionPath` if context-specific access is needed (6.4); (5) test that a user without permission is denied.

**Page/button:** `PagePermissions` uses the `<Area>/ViewPage` leaf; `*Permissions` in `Standard*Commands` use data leaves (`<Area>/View`, `<Area>/Edit/*`).

**Revisions for a new collection:** add `<Area>/Revisions` to the tree (R3.4) → add `<Area>/Revisions` at that collection's position in the revision controller's `RevisionPermissionPaths` (R6.7) → set `revisionsPermissionPath` on the QML history host.

**New product:** build the tree in Lisa → `Features.h` → `FillProduct` in `Main.cpp` → `SingleProductPermission` component (`ProductId`, `PathToPermissionsFile`) → `FeatureContainer` in `AuthenticationManager` (without it, requirements are not expanded; R5.2) → `AdminId=su`, `PatPrefix`.

## 13. Code discrepancies and open questions

Baseline: ImtCore `442be502b9`; “Fixed” means fixed in the branches listed in the header. “According to code” means inferred from code reading, not confirmed by a test.

| No. | Finding | Location | Action |
|---|---|---|---|
| 13.1 | **The mechanism was correct, but the data and groups were not.** The code means “holding A grants A's requirements” (`imtlic::CollectImpliedPermissions`, called by `IGqlContext::GetImpliedPermissions()`; `GetPermissions(productId) ∪ GetImpliedPermissions()` in `imtservergql::GetUserPermissions`). All product data was recorded in the reverse direction: `View*` → `Edit*` group (Lisa, ProLife, NeoPro, IotPlatform, Acula, `COrganizationsPermissionsProviderComp`). If expansion worked, a role with only `View*` would receive `Edit*` on the server. In addition, a group requirement granted the group path rather than its leaves; results were not limited by organization permissions; PAT expansion used all user permissions rather than scopes; and R5.4 constraints were not checked. | `imtlic/CProductInfo.cpp`, `imtgql/CGqlContext.cpp`, `imtservergql/imtservergql.h`, `*Features.h` | Fixed in code (group-target expansion to leaves, group requirements, organization boundary, PAT expansion from scopes, prohibition on dependencies within the same branch) and in Lisa, ProLife, and organization-tree data: new trees (R3.4, R3.6) use `Edit → View`. **Open:** NeoPro, IotPlatform, Acula, Agentino; audit existing roles. |
| 13.2 | Fixed: `GetUserPermissions` gets the request product's permissions (`GetPermissions(productId)`). | `imtservergql/imtservergql.h` | — |
| 13.3 | Client `PermissionsController.hasPermission` did **not** implement group checks (a held leaf `.../EditUser/ChangeUser` plus group check `.../EditUser` returned `false` on the client but `true` on the server). | `ImtCore/Qml/imtauthgui/PermissionsController.qml` | Fixed: paths only; group check means “has a leaf inside.” |
| 13.4 | PAT scope filtering used **exact matching** (`scopeSet.contains`), while scope issuance used `HasPermission`; bare identifiers produced inconsistent results. | `imtservergql/CAuthenticationManagerComp.cpp` | Fixed: use `HasPermission`; a bare scope covers nothing. |
| 13.5 | Bare identifiers are no longer supported (R4.3), but stored role, user, organization-member, PAT-scope, and organization permissions in the database are partly bare identifiers or **old paths** (`/SensorManagement/EditSensor/AddSensor`). They stop working after deployment. `imtauthdb::CPermissionPathMigrationComp` (a database migration step using `imtdb::IMigrationController`, in `ImtAuthDbPck`; not yet connected in any composition) converts bare identifiers to paths only against the current tree: old IDs (`AddSensor`) do not exist in the new tree, and short ones (`Add`) are ambiguous, so both remain unchanged. Lisa's feature catalog in the database also has the old tree. | Puma and Lisa databases, `imtauthdb/CPermissionPathMigrationComp` | **Open, before deployment:** migrate the database using an “old ID / old path → new path” mapping (available for Lisa and ProLife) and update the Lisa catalog. |
| 13.6 | Inconsistent mapping: `PackagesSetDescription` → `AddFeature` (R6.4 calls for `ChangeFeature`, as used by `ProductsSetDescription`). | `Lisa/Partitura/LisaVoce.arp/FeaturesPermissions.acc` | Clarify. |
| 13.7 | `CUserGroupCollectionControllerComp` bypasses `GroupItem`, `GetObjectData`, and `GetObjectTypeId` for **any group**, and a user delegated from another organization can run **any** group command, including change and delete. | `imtauthgql/CUserGroupCollectionControllerComp.cpp` | Bypass only reads of the user's own groups (R8.6). |
| 13.8 | `prohibitions` are stored and serialized, but are not used to calculate permissions (`CRole::GetProhibitions` returns an empty list). | `imtauth/CRole.cpp`, `CUserBaseInfo.cpp` | Implement or remove. |
| 13.9 | When `CommandId` conflicts in a joiner, the first provider silently wins. | `imtserverapp/CCommandPermissionsProviderJoinerComp.cpp` | Add diagnostics. |
| 13.10 | Fail-open: missing `CommandPermissions`/`PermissionChecker` allows the command to everyone. | `imtservergql::CheckRequestPermissions` | Validate configuration at startup. |
| 13.11 | Test `testContainerCannotBeRequirementTarget` has an incorrect name: its target `/Products/EditProducts` is a leaf, not a group. A run of the built `PermissionRequirementsTest.exe` (2026-10-16) passed 12 and failed 7; in every failed expansion test, `CollectImpliedPermissions` returned an empty list. The binary may have been built against an old `imtlic.lib` (parallel build). | `ImtCore/Tests/PermissionRequirementsTest` | Fixed: test rewritten (25 cases); `PermissionCheckTest` added (38); all pass (section 11). |
| 13.12 | `permissionPath` was checked as “declared permission is inside the path”: a group path `/Devices` passed for a command declared at `/Devices/ViewDeviceTickets`, after which **any** `/Devices` leaf sufficed. Also, the path replaced the command's AND-list with one permission. Same issue in `IsElementAccessible`. | `imtservergql/imtservergql.h`, `imtserverapp/imtserverapp.h` | Fixed (`imtauth::IsPermissionPathDeclared`): path equals a declared permission or is inside it. |
| 13.13 | Request product came from the client `productId` header; without it, permissions were collected from **all** products. Requirements always expanded from permissions across all products (`m_userInfoPtr->GetPermissions()`). | `imtservergql/CAuthenticationManagerComp.cpp`, `imtgql/CGqlContext.cpp` | Fixed: product comes from server configuration (`ProductInfo`), header is fallback only; requirements expand from request-product permissions. |
| 13.14 | According to code, Puma does not adapt the user in context to the organization (`AdaptUserForTenant` is called only when returning data), so Puma command checks include roles from all of the user's organizations. Product servers receive a user already adapted by Puma. | `imtservergql/CAuthenticationManagerComp.cpp`, Puma `PumaServerBase.acc` | Confirm with a test; adapt the user when creating the context. |
| 13.15 | `"*"` in an element or command permission list lets any user through `CheckPermissionOr`. | `imtauth/CCheckPermissionOrComp.cpp` | Prohibit it in declarations (R6.8). |
| 13.16 | Step 3 of the algorithm (superuser/owner → allow) runs before all permission checks. If a PAT belonging to an organization owner has `IsTenantOwner`, a token with narrow scopes gets full access. Not tested. | `imtservergql::CheckRequestPermissions` | Verify; a bypass must not remove scope restrictions (R9.4). |
| 13.17 | `CGuiElementRepresentationControllerComp` treats a missing `UserInfo` as a superuser (shows everything) and collects permissions across all products; an organization owner sees filtered GUI even though the server allows them. | `imtserverapp/CGuiElementRepresentationControllerComp.cpp` | Low priority: GUI is not a security boundary. |
| 13.18 | The product tree is traversed again for every request (`CollectProductRequirements` in request context, with deep feature copies). | `imtlic/CProductInfo.cpp` | Requirements are expanded on demand (R5.2), but cache the parsed product tree instead of copying it on every request. |
| 13.19 | Organization member permissions bypassed R4.1: exact `contains` checks on bare identifiers (`CanManageMembers`, member permission assignment, `HasTenantManageAccess`, organization-admin flag) and bare group IDs (`EditOrganization`, `EditOrganizationMember`) in `CTenantCollectionDocumentServiceComp`. A bare group ID granted all member management, while path-based permissions did not work. | `imtauthgql/CTenantMembershipManagerControllerComp.cpp`, `TenantAccess.cpp`, `CTenantCollectionDocumentServiceComp.cpp`, `CTenantManagerControllerComp.cpp`, `CCrossOrgGrantCollectionDocumentServiceComp.cpp` | Fixed in `feature/permission-requirements`: checks use `HasPermission` with full organization-tree leaf paths (`imtauthgql/TenantAccess.h`); a group is not a permission (R3.5). The organization tree was rebuilt (R3.6) and the member-permission editor now receives full paths (`BuildPermissionGroupsFromProvider`) instead of bare identifiers. |
| 13.20 | Revisions used one cross-cutting `RevisionManagement` group: `ViewRevisions` opened history for **all** entities, and the client checked bare `ViewRevisions`. | `CDocumentRevisionControllerComp`, `CRemoteDocumentRevisionControllerComp`, `ImtCoreVoce.arp/RevisionPermissions.acc`, `imtdocgui` | Fixed: each area has a `Revisions` subgroup (R3.4, R6.7); revision controllers in Puma, ProLife, Lisa, and Puma client use `RevisionPermissionPaths`; `RevisionPermissions.acc` was removed. Open: revision commands are declared as `Query`, but restore and delete should be `Mutation`. |
| 13.21 | Export, import, entity restore, and “view all” (`ViewAll*`) were declared with bare identifiers and no tree node, so only superusers and organization owners could use them. | `ImtUserAdministrationVoce.arp/{User,Role,Group,Tenant}Permissions.acc`, ProLife `{Account,Software,Order,Device}Permissions.acc`, `*Page.acc` | Fixed: export → `<Area>/View`, import → `<Area>/Edit/Add`, restore → `<Area>/Edit/Remove` (organizations use `/OrganizationManagement/Add` and `/Remove`), `ViewAll*` → `<Area>/ViewAll`. Only `ViewServerPage` remains bare (ProLife server settings page), intentionally available only to superusers. |
| 13.22 | In `PagesController.acc`, the Administration page used nonexistent `/Administration/EditAdministration/EditRole/AddGroup`; there was no permission to view roles in the list. | `ImtCore/Partitura/ImtGraphQlVoce.arp/PagesController.acc` | Fixed: page opens with any `/Administration/{Users,Roles,Groups}/ViewPage` (R3.4). |
| 13.23 | Member leaves do not distinguish actions: inviting, excluding, and changing a member's role, as well as changing their permissions (`UpdateMembershipPermissions`), are allowed by **any** `Members/Edit/*` leaf; checks do not use `Permissions/Change`. | `imtauthgql/CTenantMembershipManagerControllerComp.cpp` (`CanManageMembers`) | Check the leaf for the specific action. |
| 13.24 | Organization roles, groups, and users are edited through `Get/Update{Role,Group,User}{From}Representation`, declared only with product `/Administration/{Roles,Groups,Users}/...`. Member `Roles/*` and `Groups/*` permissions do not open these commands (the product organization paths in their declarations pointed to nonexistent nodes before this work and were removed). | `ImtAuthVoce.arp/{Role,User,UserGroup}CollectionDocumentService.acc`, `imtauthgql` document services | Decide whether the document services should check member leaves such as `Roles/Edit/Change` in organization context. |
| 13.25 | Search page is declared as `*` (visible to everyone, 13.15); ProLife `/SearchManagement/ViewPage` is not used in declarations. The page is shared across products, and Lisa has no such leaf. | `ImtGraphQlVoce.arp/PagesController.acc` (`SearchPage`) | Declare the page with a product leaf in each product composition. |

## 14. Current product status

| Product | Permission nodes | Requirements | Format | Direction | File |
|---|---:|---:|---|---|---|
| Lisa | 74 | 28 | Paths, R3.4 tree | `Edit` → `View`, revisions → `View` (correct) | `Lisa/Impl/LisaServer/LisaFeatures.h` |
| ProLife | 139 | 43 | Paths, R3.4 tree | `Edit` → `View`, `ViewAll`/`ViewHistory` → `View`, revisions → `View` (correct) | `ProLife/Impl/ProLifeServer/ProLifeFeatures.h` |
| IotPlatform | 89 | 7 | Paths | `View*` → `Edit*` | `IotPlatform/Impl/IotPlatformServerExe/IotPlatformFeatures.h` |
| NeoPro | 35 | 4 | Paths | `View*` → `Edit*` | `NeoPro/Impl/NeoProServer/NeoProFeatures.h` |
| Acula | 21 | 4 | Paths | `View*` → `Edit*` | `Acula/Impl/AculaServer/AculaFeatures.h` |
| Agentino | 18 | 0 | — | — | `Agentino/Impl/AgentinoServer/AgentinoFeatures.h` |

## 15. Key files

| Topic | File |
|---|---|
| Comparison rule | `ImtCore/Include/imtauth/imtauth.h` (`HasPermission`) |
| Command checks | `ImtCore/Include/imtservergql/imtservergql.h`, `CPermissibleGqlRequestHandlerComp.*` |
| Checkers | `ImtCore/Include/imtauth/CCheckPermissionCompBase.cpp`, `CCheckPermissionOrComp.cpp` |
| GUI elements | `ImtCore/Include/imtserverapp/imtserverapp.h` (`IsElementAccessible`), `imtservergql/CCommandsControllerComp.cpp` |
| Context and requirements | `ImtCore/Include/imtgql/CGqlContext.*`, `imtlic/CProductInfo.cpp`, `imtlic/IFeatureInfo.h` |
| Request context, PAT, organization | `ImtCore/Include/imtservergql/CAuthenticationManagerComp.cpp` |
| User permissions | `imtauth/CUserBaseInfo.cpp`, `CUserInfo.cpp`, `CRole.cpp`, `imtauthgql/imtauthgql.h` |
| GraphQL permission tree | `ImtCore/Sdl/imtauth/1.0/Permissions.sdl`, `imtauthgql/CPermissionsProviderComp.cpp` |
| Command permissions (Partitura) | `ImtCore/Partitura/ImtCoreVoce.arp/*Permissions.acc`, `ImtAuthVoce.arp/ObjectCollectionPermissions.acc`, `Lisa/Partitura/LisaVoce.arp/*Permissions.acc` |
| Client | `ImtCore/Qml/imtauthgui/PermissionsController.qml`, `PermissionsProvider.qml`, `Qml/imtlicgui/FeatureEditor.qml` |
| Revisions | `ImtCore/Include/imtservergql/imtservergql.h` (`CheckRevisionPermissions`), `CDocumentRevisionControllerComp.*`, `imtclientgql/CRemoteDocumentRevisionControllerComp.*`, `Qml/imtdocgui/DocumentHistoryView.qml` |
| Organization tree | `ImtCore/Include/imtauth/COrganizationsPermissionsProviderComp.cpp`, `imtauthgql/TenantAccess.h` |
| Tests | `ImtCore/Tests/PermissionRequirementsTest`, `ImtCore/Tests/PermissionCheckTest`, Puma `Tests/TenantApiPostman`, ProLife `Tests/ProLifeApiPostman`, `ImtCore/Tests/GuiTestKit`, `Lisa/Tests/LisaGui` |
