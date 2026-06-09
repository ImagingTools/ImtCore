# Cross-Org Grant: Delegated Access & Permission Checking

This document describes how `CrossOrgGrant` is integrated into delegated
cross-organization access, organization visibility, and permission checking.

## Overview

A `CrossOrgGrant` delegates a scoped set of roles from a **source tenant**
(the organization granting access) to a **target tenant** (the organization
receiving access). Users of the target tenant can then operate inside the
source tenant, but only within the roles delegated by the grant.

The grant store and lifecycle (create / revoke / expiry) are provided by
`imtauth::ICrossOrgGrant` and implemented in
`imtauth::CCrossOrgGrantManagerComp`. A grant is *effective* only when it is
active and not expired.

## Resolution helpers

In addition to grant CRUD, `ICrossOrgGrant` exposes resolution helpers that the
UI, navigation, and permission-checking layers consume:

| Method | Purpose |
| --- | --- |
| `HasAccess(source, target, roleId)` | True when the target tenant holds `roleId` in the source tenant through an effective grant. An empty `roleId` always returns true. |
| `GetDelegatedSourceTenants(target)` | Distinct source tenants reachable by the target tenant through at least one effective grant. |
| `GetGrantedRoles(source, target)` | Union of role IDs delegated from the source to the target across all effective grants (deduplicated). |
| `IsDelegatedAccess(source, target)` | True when at least one effective grant exists between source and target. |

All helpers ignore revoked and expired grants, so callers never have to repeat
the active/expiry checks.

## How the layers use these helpers

### 1. Organization visibility (switcher)

The organization switcher combines a user's direct memberships with the
delegated organizations returned by `GetDelegatedSourceTenants(currentTenantId)`.
A delegated organization is presented distinctly from membership organizations,
because access to it is constrained by the grant.

### 2. Delegated context switch

When a user switches context into a delegated organization, the switch is valid
only if `IsDelegatedAccess(targetOrg, currentTenant)` is true. The active roles
for that delegated session are `GetGrantedRoles(targetOrg, currentTenant)`, and
the UI surfaces both the delegated nature of the session and the active roles.

### 3. Permission checking

Permission checks for a delegated session resolve against the roles returned by
`GetGrantedRoles` (the scope of the grant), rather than the user's own roles in
the source tenant. Access to a resource or action is allowed only when one of
the delegated roles permits it, which can be checked directly with `HasAccess`.

## Extensibility

The model is designed to support future extensions without breaking the
resolution contract:

- **Expiry** is already honoured by every helper through the effectiveness check.
- **Multi-grant** is supported: roles and source tenants are unioned across all
  effective grants between two tenants.
- **Entity-type scoping** and **trust chains** can be layered on by extending
  `CrossOrgGrantInfo` and the effectiveness/resolution logic while keeping the
  existing helper signatures stable.
