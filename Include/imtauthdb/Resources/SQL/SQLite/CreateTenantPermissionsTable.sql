CREATE TABLE IF NOT EXISTS "TenantPermissions"
(
    "TenantId"     TEXT NOT NULL,
    "PermissionId" TEXT NOT NULL,
    PRIMARY KEY ("TenantId", "PermissionId"),
    FOREIGN KEY ("TenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);
CREATE INDEX IF NOT EXISTS "IX_TenantPermissions_TenantId" ON "TenantPermissions" ("TenantId");
