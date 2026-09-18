# Password Policy (IEC 62443-4-2 CR 1.7)

ImtCore provides a configurable password policy that implements the capability requirements of
IEC 62443-4-2 CR 1.7 "Strength of password-based authentication" for security levels SL-C 1 to SL-C 4:

- **SL-C 1 / SL-C 2** — configurable password strength according to internationally recognized
  password guidelines (NIST SP 800-63B oriented: length first, optional character classes,
  rejection of the login as password, optional blocklist of common/compromised passwords).
- **SL-C 3** — protection against password reuse for a configurable number of generations,
  minimum and maximum password lifetime restrictions for human users, and prompting the user
  to change the password a configurable time before expiration.
- **SL-C 4** — minimum and maximum password lifetime restrictions for *all* password-based
  accounts, including the superuser.

## Components

| Component | Location | Purpose |
| --- | --- | --- |
| `imtauth::IPasswordPolicy` | `Include/imtauth/IPasswordPolicy.h` | Policy interface (strength, history, lifetime) |
| `imtauth::CPasswordPolicyControllerComp` | `Include/imtauth/CPasswordPolicyControllerComp.{h,cpp}` | Configurable policy implementation (`PasswordPolicy` component in `ImtAuthPck`) |
| `imtauthgql::CUserControllerComp` | `Include/imtauthgql/CUserControllerComp.{h,cpp}` | Enforces strength, reuse and minimum age on `ChangePassword`, `RegisterUser` and `CreateSuperuser` |
| `imtauthgql::CAuthorizationControllerComp` | `Include/imtauthgql/CAuthorizationControllerComp.{h,cpp}` | Evaluates password lifetime on `Authorization`/`UserToken` |

The `PasswordPolicy` reference on both controllers is **optional**. If it is not wired,
the behavior is identical to previous releases (no policy enforcement).

## Configuration attributes of `PasswordPolicy`

| Attribute | Type | Default | Description |
| --- | --- | --- | --- |
| `MinPasswordLength` | Integer | 8 | Minimum password length. NIST SP 800-63B requires at least 8; 12 or more is recommended. |
| `MaxPasswordLength` | Integer | 128 | Maximum password length (0 = unlimited). Keep large to support passphrases. |
| `RequireLowercase` | Boolean | false | Require at least one lowercase letter. |
| `RequireUppercase` | Boolean | false | Require at least one uppercase letter. |
| `RequireDigit` | Boolean | false | Require at least one digit. |
| `RequireSpecialChar` | Boolean | false | Require at least one special character. |
| `RejectLoginAsPassword` | Boolean | true | Reject passwords equal to the login (case-insensitive). |
| `BlocklistFilePath` | Text | (empty) | Path to a text file with blocked passwords, one per line (e.g. a list of commonly used or compromised passwords). Comparison is case-insensitive. |
| `PasswordHistoryDepth` | Integer | 5 | Number of previous password generations that may not be reused (0 = disabled). |
| `MinPasswordAge` | Integer | 0 | Minimum password age in days before the password may be changed again (0 = disabled). Prevents rapid cycling to defeat the history check. Not applied to administrator-initiated changes or required changes. |
| `MaxPasswordAge` | Integer | 0 | Maximum password age in days before the password expires (0 = disabled). |
| `ExpirationWarningPeriod` | Integer | 14 | Number of days before expiration during which the user is prompted to change the password (0 = disabled). |

These defaults live in `imtauth::PasswordPolicyDefaults` and are used both in `I_ASSIGN` and
wherever an unset attribute is read. ACF applies an `I_ASSIGN` default only to **obligatory**
attributes, so for these optional ones the component itself has to supply the value — a
fallback that disagrees with the declared default silently changes the policy.

Lifetime restrictions (`MinPasswordAge`, `MaxPasswordAge`) are disabled by default (value 0),
so no existing password expires because the policy component was added.

Strength and reuse checks, however, are **active as soon as the component is wired**, with their
defaults: a password shorter than 8 characters, a password equal to the login, and reuse of one of
the last 5 passwords are rejected. Existing accounts keep working and are never locked out by this,
but their owners may be unable to set a password that was acceptable before. Set
`MinPasswordLength` and `PasswordHistoryDepth` explicitly if a deployment needs the previous
behavior.

## Behavior

### Password change (`ChangePassword` mutation)

1. The new password is validated against the strength rules. On failure the mutation returns
   `success = false`, a human-readable `message` and the machine-readable `violatedRules` list
   (`MinLength`, `MaxLength`, `LowercaseRequired`, `UppercaseRequired`, `DigitRequired`,
   `SpecialCharRequired`, `LoginAsPassword`, `BlocklistedPassword`).
2. The minimum password age is enforced (`violatedRules = [MinPasswordAge]`), except for
   administrator-initiated changes and required changes (expired/reset passwords).
3. Reuse of any of the last `PasswordHistoryDepth` passwords (including the current one) is
   rejected (`violatedRules = [PasswordReused]`).
4. On success the previous password hash is prepended to the password history, the history is
   trimmed to the configured depth, the password change timestamp is updated and the
   "must change password" flag is cleared.

### Login (`Authorization` / `UserToken` queries)

- If the password exceeded `MaxPasswordAge`, or a password change is explicitly required for
  the user, the login is denied and the payload carries `passwordExpired = true`. The client
  should route the user to the change-password flow (the change is authorized with the old
  password via `ChangePassword`).
- If the password expires within `ExpirationWarningPeriod` days, the login succeeds and the
  payload carries `passwordExpiresInDays` so clients can prompt the user to change the
  password before it expires.

### Policy publication (`GetPasswordPolicy` query)

`GetPasswordPolicy` returns the configured strength rules (`minLength`, `maxLength`,
`requireLowercase`, `requireUppercase`, `requireDigit`, `requireSpecialChar`,
`rejectLoginAsPassword`, `blocklistUsed`) and `historyDepth`. It carries no secrets and is
reachable without authentication, because the superuser-creation and password-recovery flows
need it before a session exists.

Clients use it to check a password candidate **before** sending it, so a rejection does not
require a round trip, and to render the concrete requirements next to the input. The check is
a convenience only — the server-side check stays authoritative, and the blocklist is never
published (it is only evaluated on the server).

On the client the policy follows the same contract/transport split as `PermissionsProvider`:
`imtauthgui/PasswordPolicyController.qml` is the transport-free contract (rules, the mirrored
rule evaluation and the texts derived from a `violatedRules` list), and
`imtguigql/GqlBasedPasswordPolicyProvider.qml` derives from it and overrides `load()` with the
actual `GetPasswordPolicy` request, so `imtauthgui` stays free of GQL.

An instance is injected into the views: `GqlBasedProfileApiClient` and
`GqlBasedUserAdministrationApiClient` expose it as `passwordPolicy`, while the two pre-login
screens (`SuperuserPasswordPage`, `PasswordRecoveryDialog`) create their own. A view without
an injected policy keeps working — only the extra checks are skipped.

### User data model

`imtauth::CUserInfo` stores the password history, the UTC timestamp of the last password
change and the "must change password" flag. Serialization is guarded by the archive version,
so previously persisted user collections load unchanged (empty history, unknown change time).

The version gate in `CUserInfo::Serialize` (`imtCoreVersion >= 23244`) must be **at or above
the ImtCore version number the change is released with**. The number is
`git rev-list --count origin/main + 10000` (see `Build/Git/UpdateVersion.sh`). A gate below
the released version makes archives written by earlier builds — which do not contain the
fields — be read as if they did. Re-check the value before merging if `main` advanced in the
meantime.

Users without a stored password change timestamp (created before this feature) are **not**
locked out: expiration is enforced starting from their next password change.

## Externally managed users

Password quality and lifetime of externally managed accounts are governed by the external
system ("integrate into a system that provides the capability" clause of CR 1.7):

- **LDAP users** (`CLdapCredentialControllerComp`) — password policy of the directory applies;
  the lifetime check is skipped for accounts authenticated against a non-empty system ID, and
  password changes for external users are rejected by `ChangePassword` as before.
- **Personal access tokens** and **JWT sessions** have their own lifetime mechanisms
  (`ExpiresAt`, `JwtLifetime`/`RefreshTokenLifetime`).

## Wiring example

The `PasswordPolicy` element is wired in
`Partitura/ImtAuthVoce.arp/AuthorizationController.acc` (lifetime evaluation at login) and
`Partitura/ImtUserAdministrationVoce.arp/UsersPage.acc` (strength/history/minimum-age
enforcement on password changes). All policy limits are exported as configuration attributes
so integrators can tune them per deployment and per target security level.

## Tests

`Tests/PasswordPolicyTest` covers the strength acceptance/rejection matrix, history reuse
across generations, history trimming, minimum-age rejection, maximum-age expiry, the warning
window, the extended `CUserInfo` data model (copy/clone/equality/reset) and the serialization
round trip for both a current archive and a pre-feature one.
