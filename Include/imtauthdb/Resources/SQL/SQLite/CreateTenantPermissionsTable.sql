CREATE TABLE IF NOT EXISTS "TenantPermissions"
(
    "TenantId"     TEXT NOT NULL,
    "ProductId"    TEXT NOT NULL,
    "PermissionId" TEXT NOT NULL,
    PRIMARY KEY ("TenantId", "ProductId", "PermissionId"),
    FOREIGN KEY ("TenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);
CREATE INDEX IF NOT EXISTS "IX_TenantPermissions_TenantId" ON "TenantPermissions" ("TenantId");
CREATE INDEX IF NOT EXISTS "IX_TenantPermissions_TenantId_ProductId" ON "TenantPermissions" ("TenantId", "ProductId");
