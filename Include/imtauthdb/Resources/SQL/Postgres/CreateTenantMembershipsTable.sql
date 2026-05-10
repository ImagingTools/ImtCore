CREATE TABLE IF NOT EXISTS "${TableScheme}"."TenantMemberships"
(
    "Id"          UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "UserId"      UUID NOT NULL,
    "TenantId"    UUID NOT NULL,
    "Role"        INTEGER NOT NULL DEFAULT 2,
    "IsActive"    BOOLEAN NOT NULL DEFAULT TRUE,
    "JoinedAt"    TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC'),
    "UpdatedAt"   TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC'),
    CONSTRAINT "FK_TenantMemberships_TenantId" FOREIGN KEY ("TenantId") REFERENCES "${TableScheme}"."Tenants" ("Id") ON DELETE CASCADE,
    CONSTRAINT "UQ_TenantMemberships_User_Tenant" UNIQUE ("UserId", "TenantId")
);
CREATE INDEX IF NOT EXISTS "IX_TenantMemberships_UserId" ON "${TableScheme}"."TenantMemberships" ("UserId");
CREATE INDEX IF NOT EXISTS "IX_TenantMemberships_TenantId" ON "${TableScheme}"."TenantMemberships" ("TenantId");
CREATE INDEX IF NOT EXISTS "IX_TenantMemberships_Role" ON "${TableScheme}"."TenantMemberships" ("Role");
