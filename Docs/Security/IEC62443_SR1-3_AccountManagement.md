# IEC 62443-3-3 SR 1.3 — Account Management

## Requirement

> Components shall provide the capability to support the management of all accounts directly or
> integrated into a system that manages accounts according to IEC 62443-3-3 SR 1.3.

SR 1.3 requires support for the management — creation, activation, modification, disabling and
removal — of all account types (users, groups, roles and service accounts), either directly or via
integration with an external account management system.

## How ImtCore satisfies SR 1.3

### Direct account management (`imtauth` / `imtauthgql`)

| Capability | Implementation |
|---|---|
| Create account | `imtauth::IUserManager::CreateUser`; GraphQL `UserAdd`, `RegisterUser` mutations (`Sdl/imtauth/1.0/Users.sdl`) |
| Modify account | `UserUpdate` mutation; document-based flow via `UserCollectionDocumentService.sdl` (`GetUserRepresentation` / `UpdateUserFromRepresentation`) |
| Change credentials | `ChangePassword` mutation; `imtauth::IUserManager::ChangeUserPassword` |
| Disable / enable account | `enabled` field on `UserData` / `UserItemData` (`Users.sdl`), backed by `imtauth::IUserInfo::IsEnabled()` / `SetEnabled()`; reserved for the superuser (see below) and editable in the user management UI (`Qml/imtauthgui/UserGeneralEditor.qml`) |
| Remove account | `imtauth::IUserManager::RemoveUser`; user collection document/collection services (delete operations of the collection framework) |
| Group management | `Groups.sdl`, `imtauth::IUserGroupManager`, group collection document service |
| Role management | `Roles.sdl`, role collection document service, `AddRolesToUser` / `RemoveRolesFromUser` |
| Service accounts / non-interactive access | Personal access tokens (`PersonalAccessTokens.sdl`, `imtauthgql::CPersonalAccessTokenControllerComp`) with scoped permissions |
| Administrative bootstrap | `CheckSuperuserExists` / `CreateSuperuser` (`Users.sdl`) |
| Account verification | Email-based verification (`imtauth::CUserVerificationControllerComp`) |
| Session management | `Sessions.sdl`, `imtauth::IJwtSessionController` |

### Who may change the account state

Enabling and disabling an account is reserved for the superuser (`imtauth::IUserInfo::IsAdmin()`,
the account whose login matches the `AdminId` attribute — `su` in the standard configuration). Every
server-side write path that accepts the `enabled` field guards it with
`imtauthgql::IsSuperuserRequest()`:

- `imtauthgql::CUserCollectionDocumentServiceComp::OnUpdateUserFromRepresentation` — the path used
  by the user administration UI; a non-superuser request that flips the flag is answered with a
  `Failed` document operation status and an explanatory message.
- `imtauthgql::CUserCollectionControllerComp::UpdateObjectFromRepresentationRequest` — the
  `UserUpdate` mutation.
- `imtauthgql::CUserCollectionControllerComp::CreateObjectFromRequest` — the `UserAdd` mutation;
  only the superuser may create an account in the disabled state.
- `imtauthgql::CUserControllerComp::OnRegisterUser` — self-registration always creates an enabled
  account, regardless of what the (unauthenticated) request sends.

A request that submits the `enabled` field unchanged is always accepted, so ordinary user
administration by non-superusers is unaffected. In the UI the switch is shown read-only to everyone
but the superuser (`Qml/imtauthgui/UserGeneralEditor.qml`), so the account state stays visible
without being editable.

### Enforcement of the account enabled state

A disabled account is denied access at every entry point without being removed:

- **Interactive login** — `imtauthgql::CAuthorizationControllerComp::OnAuthorization` and
  `OnUserToken` reject authorization for accounts where `IsEnabled()` returns `false`. The denial is
  recorded in the server log.
- **LDAP login** — `imtauthgql::CLdapAuthorizationControllerComp::OnAuthorization` applies the same
  rejection to accounts joined from the external directory.

The state check runs **after** the credentials are verified, which decides what the caller is told:

| Credentials | Account state | Response |
|---|---|---|
| wrong | any | `Invalid login or password` — account existence and state are never disclosed to a caller who did not prove ownership |
| correct | disabled | `AuthorizationPayload.accountDisabled = true`, no token; the client shows "This account has been deactivated. Please contact your administrator." |
| correct | enabled | normal token response |

Naming the state after a correct password discloses nothing the caller has not already proven, and
it keeps a legitimate user from mistaking deactivation for a broken password. Ordering the check
this way is what prevents a login attempt from being used as an account-state probe.
- **Existing sessions and tokens** — `imtservergql::CAuthenticationManagerComp::CreateGqlContext`
  rejects every request (JWT session or personal access token) made on behalf of a disabled
  account with a `FORBIDDEN` status, so disabling an account immediately invalidates its active
  sessions and personal access tokens.

The account state is persisted with the user object (`imtauth::CUserInfo::Serialize`) and defaults
to *enabled* for data written by earlier versions, preserving backward compatibility.

Per-identity-system activation is additionally available through
`IUserInfo::SystemInfo::enabled`, which controls which authentication systems (internal, LDAP, …)
may be used by an account. The account-level `enabled` flag overrides all system-level flags.

### Integration with external account management systems

ImtCore integrates with LDAP / Active Directory:

- `imtauthgql::CLdapAuthorizationControllerComp` — authenticates users against the directory and
  joins them into the user collection on first login.
- `imtauth::CLdapCredentialControllerComp` — validates directory credentials.
- `imtauth::CLdapUserCollectionControllerComp` / `CLdapUserCollectionJoinerComp` — synchronize
  directory accounts into the user collection.
- `UserData.systemInfos` (`SystemInfo` in `Users.sdl`) records which identity system each account
  belongs to; roles and groups are assigned locally, so authorization stays under local control
  even for externally managed accounts.

### Auditability

Account lifecycle operations are performed through the user collection and document services,
whose changes are tracked by the existing change-tracking components
(`imtauth::IUserActionManager`, `imtauth::CUserChangeGeneratorComp`) and the revision framework.
Denied authorization attempts for disabled accounts are recorded in the server log.

## Deployment options

1. **Direct management** — accounts, groups and roles are created and maintained in the ImtCore
   user collection via the GraphQL API and the administration UI.
2. **External directory integration** — accounts are authenticated against LDAP / Active
   Directory and joined into the user collection; local disabling and removal remain available for
   the joined account, and per-system activation controls which directory may authenticate it.

## Related tests

- `Tests/UserAccountStateTest` — enable/disable lifecycle of the user account data model.
- `Tests/PersonalAccessTokenTest` — creation, validation, revocation and expiration of personal
  access tokens (service account credentials).
