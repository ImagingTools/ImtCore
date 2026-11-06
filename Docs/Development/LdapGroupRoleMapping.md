# Active Directory group-to-role mapping

## Deployment

This opt-in feature extends `imtauthgql::CLdapAuthorizationControllerComp` on Windows.
No Puma files are changed. Puma's `Partitura/PumaVoce.arp/Authorization.acc` already
instantiates this component; configure its new attribute in the consuming product.
The shared `ImtAuthVoce.arp/AuthorizationController.acc` also exposes the LDAP
controller and these optional settings.

1. Connect the existing persistent `UserCollection` and `RoleCollection`,
   `LdapEnableableParam`, and LDAP credential controller (`SystemId = Ldap`).
   The collection's persistent user factory must have its `RoleProvider` connected
   to this role collection, as for manually assigned roles. A mapped sign-in with
   no user role provider is rejected rather than returning inconsistent permissions.
2. Set `LdapGroupSettingsFile` to an **absolute**, service-writable file path in a
   persistent, access-controlled configuration directory. The directory must exist.
   Leave it empty to disable mapping administration and retain legacy behavior.
   Precreate `{"version":1,"products":{}}`, or save initial disabled settings as a
   local superuser. Administration can bootstrap a missing file; LDAP sign-ins
   with a configured file refuse missing/inaccessible/malformed files rather than
   trusting an incomplete configuration. Protect the file and its parent from untrusted writes.
3. Set `AdministrationView.ldapGroupMappingAvailable` (or the same property of
   `AdministrationUi`) to `true`. Only logged-in superusers see **Directory groups**.
   Route its requests to the same LDAP authorization component. It recognizes the
   three administration commands independently of the legacy `CommandIds` list.
4. Run the server as a domain identity with directory read rights. Discovery uses
   the service identity's `defaultNamingContext`. Bindings require integrated
   authentication, signing and sealing. A domain controller and global catalog
   must be reachable. Non-Windows mapping/discovery fails closed.
5. In Directory groups: load the product's existing roles, enter a group-name
   pattern such as `Product-*`, discover groups, select each group and check
   **all** desired roles, enable mapping, then save. Only `*` is a wildcard;
   other LDAP filter metacharacters are escaped. Distribution groups are excluded.
   Groups are identified by immutable SIDs, not display names. An unavailable group
   remains visible by SID so its mappings can be removed. **Clear mappings** also
   permits recovery after deleting a mapped role.

The shared LDAP controller remains disabled unless `LdapEnableableParam` is enabled.
With mapping disabled, existing default-role provisioning and manual assignments
remain in effect. Mapping adds roles; it does not replace the legacy default role.
Review default-role permissions before enabling directory-controlled access.

## Authorization and persistence

Both `Authorization` and `UserToken` use LDAP provisioning and the common pre-token
refresh hook. After credentials and enabled-account checks, AD `tokenGroups` is
read using the account SID. It includes nested security groups and the primary
group, not merely direct `memberOf` entries. The role set is the union of **every**
matching mapped security group. The discovered pattern is checked again on login,
so a renamed group that no longer matches cannot continue adding grants.

Mappings are product-scoped and can reference only existing roles of that product.
Every enabled mapping's roles are validated before login/save; an unreadable/deleted/foreign
role denies login until the superuser corrects the configuration. Reading the
directory, settings, or user, and writing the grants must succeed before a token
is issued. A successful empty membership result removes directory grants; a
failed read is never treated as an empty result.
Disabled mappings do not resolve directory membership or stale role references,
so disabling them can revoke source-owned grants even after a role is deleted.

Directory grants are source-owned, separate from manual roles, including when
both sources grant the same role. The existing user `Products` archive stores
them under reserved keys `urn:imt:directory-roles:<productId>`. No schema migration
or new archive version is required. Do not use this namespace for real products.
`GetRoles(productId)` remains the manual role list so editing/saving a user cannot
accidentally turn directory grants into permanent manual roles. `GetPermissions`
unions manual and directory roles (deduplicating permissions), and validates each
directory role's product and inherited-role closure again, failing closed on an
unreadable or foreign-product parent. Cloning, serialization and subsequent stored-user
permission queries retain directory grants. Disabling mapping or removing group
membership removes only directory grants at the next successful sign-in.
Tenant-aware role/permission queries include directory roles through the same
binding filters as manual roles; tenant adaptation changes a clone, not the
persisted source assignments.

Settings use versioned JSON, one document for all products, atomic `QSaveFile`
replacement, an in-process mutex and an interprocess save lock. Saves preserve
other products. Deploy one authorization component per collection/settings file;
there is no compare-and-swap user-update API to coordinate independent processes
editing the same user. Do not run independent grant writers against the same
collection without deployment-level synchronization.

### Session limitation

Membership/configuration refresh occurs **at credential sign-in**, for the requested
product. Existing JWTs, refresh-token operations, personal access tokens and other
products' saved grants do not trigger directory queries. Existing sessions can
retain prior rights until sign-in/session revocation; directory outages refuse
new mapped sign-ins but do not invalidate existing sessions. Revoke sessions and
tokens operationally when immediate removal is required. Disabling LDAP or clearing
the settings-file attribute removes saved directory grants on the next product
sign-in, not immediately. Do not downgrade a deployment with active directory
grants: older binaries do not understand their source namespace.

## Administration API

The contract is in `Sdl/imtauth/1.0/LdapGroups.sdl`. Every operation requires a
resolved superuser context on the server; a hidden UI is not the security boundary.
Queries cannot save settings. `SaveLdapGroupSettings` requires a mutation.

- `LdapGroupSettings(input: {productId})`: JSON-encoded `settings` and `roles`
  (`id` is the role **object ID**, `name` its display name).
- `DiscoverLdapGroups(input: {productId, pattern})`: matching `groups` (`id` is SID).
- `SaveLdapGroupSettings(input: {productId, settings})`: `ok` and saved `settings`.

The `settings` string encodes:

```json
{
  "enabled": true,
  "pattern": "Product-*",
  "mappings": {
    "S-1-5-21-100-200-300-400": [
      "existing-product-role-object-id",
      "another-existing-product-role-object-id"
    ]
  }
}
```

The server file wraps these objects as
`{"version":1,"products":{"<productId>":<settings>}}`.
Discovery is paged; exceeding 10,000 groups or an incomplete directory read
returns an error, never a partial trusted group list.

## Validation

`Tests/UserControllerTest/CLdapGroupMappingTest` extends the existing QtTest
executable. It covers both login commands, stored permission queries, all-role
union, product isolation, membership revocation, manual-role preservation,
archive/clone/reset, disabled/corrupt settings, failed persistence, superuser
checks, cross-product role rejection, tenant isolation and settings persistence.

Build `UserControllerTest` with the repository's existing CMake/ACF setup and run
its standard test executor. Windows integration still requires a real domain:
verify nested/primary membership, signed/sealed directory binding, a group rename,
directory/global-catalog failure, and both login commands before production rollout.
