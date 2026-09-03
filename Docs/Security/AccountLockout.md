# Account Lockout (Unsuccessful Login Attempts)

This document describes the account lockout capability of the ImtCore authentication
components. It implements the requirement that a component providing an authentication
capability shall:

a) enforce a limit of a configurable number of consecutive invalid access attempts by any
   user (human, software process or device) during a configurable time period; and
b) deny access for a specified period of time or until unlocked by an administrator when
   this limit has been reached. An administrator may unlock an account prior to the
   expiration of the timeout period.

## Components

### `imtauth::IAccountLockoutController`

Interface (`Include/imtauth/IAccountLockoutController.h`) providing the lockout capability:

- `IsAccountLocked(login, remainingLockoutSecsPtr)` — check whether access for an account
  must currently be denied.
- `RegisterFailedAttempt(login)` — record an invalid access attempt; returns `true` when
  the account is (now) locked.
- `RegisterSuccessfulAttempt(login)` — reset the consecutive failed attempt counter after
  a successful authentication.
- `UnlockAccount(login)` — unlock an account prior to the expiration of the lockout
  period (administrator action).

### `imtauth::CAccountLockoutControllerComp`

Thread-safe in-memory implementation (`Include/imtauth/CAccountLockoutControllerComp.h/.cpp`),
registered in `ImtAuthPck` as `AccountLockoutController`. Configurable attributes:

| Attribute | Default | Description |
|---|---|---|
| `MaxFailedAttempts` | `5` | Maximum number of consecutive invalid access attempts before the account is locked. `0` disables the lockout. |
| `FailedAttemptsPeriod` | `300` (5 min) | Time period in seconds within which consecutive invalid attempts are counted. Attempts older than this period are discarded. |
| `LockoutDuration` | `900` (15 min) | Time period in seconds for which access is denied after the limit has been reached. `0` locks the account until it is unlocked by an administrator. |

Lockout and unlock events are reported via the component log.

## Enforcement

`imtauthgql::CAuthorizationControllerComp` accepts an optional `AccountLockoutController`
reference. When set, both the `Authorization` and `UserToken` GraphQL flows:

1. Deny access with an "account is temporarily locked" error while the account is locked,
   without evaluating the supplied credentials.
2. Register every invalid access attempt (unknown login or wrong password) with the
   lockout controller.
3. Reset the failed attempt counter after a successful authentication.

The `ImtAuthVoce.arp/AuthorizationController.acc` partitura wires an
`AccountLockoutController` instance into the authorization controller and exports the
configuration attributes (`MaxFailedAttempts`, `FailedAttemptsPeriod`, `LockoutDuration`)
as well as the component itself for application-level wiring.

## Administrator Unlock

The `UnlockUser` GraphQL mutation (`Sdl/imtauth/1.0/Users.sdl`) allows an administrator to
unlock an account prior to the expiration of the lockout period:

```graphql
mutation {
    UnlockUser(input: { login: "user" }) {
        success
        message
    }
}
```

It is handled by `imtauthgql::CUserControllerComp` (optional `AccountLockoutController`
reference) and is subject to the standard permission checks — unlike e.g. `RegisterUser`,
it is not accessible anonymously.

An account also unlocks automatically after `LockoutDuration` seconds when a non-zero
duration is configured.

## Notes

- The lockout state is kept in memory; a service restart resets counters and locks.
- `CLdapAuthorizationControllerComp` (LDAP login flow) does not yet consult the lockout
  controller; LDAP servers usually enforce their own lockout policy.
