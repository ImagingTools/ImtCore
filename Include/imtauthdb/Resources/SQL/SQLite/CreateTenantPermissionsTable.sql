CREATE TABLE IF NOT EXISTS "TenantPermissions"
(
    "TenantId"     TEXT NOT NULL,
    "ProductId"    TEXT NOT NULL,
    "PermissionId" TEXT NOT NULL,
    PRIMARY KEY ("TenantId", "ProductId", "PermissionId"),
    FOREIGN KEY ("TenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);
