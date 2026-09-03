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

All lifetime restrictions are disabled by default (value 0), so existing deployments
are unaffected unless the policy is explicitly configured.

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

### User data model

`imtauth::CUserInfo` stores the password history, the UTC timestamp of the last password
change and the "must change password" flag. Serialization is guarded by the archive version,
so previously persisted user collections load unchanged (empty history, unknown change time).

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
window and the extended `CUserInfo` data model (copy/clone/equality/reset).
