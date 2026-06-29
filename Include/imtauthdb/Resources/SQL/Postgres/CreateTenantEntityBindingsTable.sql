CREATE TABLE IF NOT EXISTS "${TableScheme}"."TenantEntityBindings"
(
    "Id"              UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "TenantId"        UUID NOT NULL,
    "EntityType"      TEXT NOT NULL,
    "EntityId"        UUID NOT NULL,
    "CreatedAt"       TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC'),
    "CreatedByUserId" UUID,
    CONSTRAINT "UQ_TenantEntityBindings_Tenant_Type_Entity" UNIQUE ("TenantId", "EntityType", "EntityId")
);
CREATE INDEX IF NOT EXISTS "IX_TenantEntityBindings_TenantId" ON "${TableScheme}"."TenantEntityBindings" ("TenantId");
CREATE INDEX IF NOT EXISTS "IX_TenantEntityBindings_EntityType_EntityId" ON "${TableScheme}"."TenantEntityBindings" ("EntityType", "EntityId");
CREATE INDEX IF NOT EXISTS "IX_TenantEntityBindings_EntityId" ON "${TableScheme}"."TenantEntityBindings" ("EntityId");
