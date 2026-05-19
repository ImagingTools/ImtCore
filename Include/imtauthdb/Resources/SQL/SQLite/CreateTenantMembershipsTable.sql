CREATE TABLE IF NOT EXISTS "TenantMemberships"
(
    "Id"          TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "UserId"      TEXT NOT NULL,
    "TenantId"    TEXT NOT NULL,
    "Role"        TEXT NOT NULL,
    "IsActive"    INTEGER NOT NULL DEFAULT 1,
    "JoinedAt"    TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "UpdatedAt"   TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY ("TenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);
CREATE UNIQUE INDEX IF NOT EXISTS "UQ_TenantMemberships_User_Tenant" ON "TenantMemberships" ("UserId", "TenantId");
CREATE INDEX IF NOT EXISTS "IX_TenantMemberships_UserId" ON "TenantMemberships" ("UserId");
CREATE INDEX IF NOT EXISTS "IX_TenantMemberships_TenantId" ON "TenantMemberships" ("TenantId");
CREATE INDEX IF NOT EXISTS "IX_TenantMemberships_Role" ON "TenantMemberships" ("Role");
