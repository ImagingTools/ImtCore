CREATE TABLE IF NOT EXISTS "${TableScheme}"."TenantPermissions"
(
    "TenantId"     UUID NOT NULL,
    "ProductId"    TEXT NOT NULL,
    "PermissionId" TEXT NOT NULL,
    CONSTRAINT "PK_TenantPermissions" PRIMARY KEY ("TenantId", "ProductId", "PermissionId"),
    CONSTRAINT "FK_TenantPermissions_TenantId" FOREIGN KEY ("TenantId") REFERENCES "${TableScheme}"."Tenants" ("Id") ON DELETE CASCADE
);
CREATE INDEX IF NOT EXISTS "IX_TenantPermissions_TenantId" ON "${TableScheme}"."TenantPermissions" ("TenantId");
CREATE INDEX IF NOT EXISTS "IX_TenantPermissions_TenantId_ProductId" ON "${TableScheme}"."TenantPermissions" ("TenantId", "ProductId");
