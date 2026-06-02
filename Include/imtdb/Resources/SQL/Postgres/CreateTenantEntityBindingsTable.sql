CREATE TABLE IF NOT EXISTS "${TableScheme}"."TenantEntityBindings"
(
    "Id"              TEXT PRIMARY KEY,
    "TenantId"        TEXT NOT NULL,
    "EntityType"      TEXT NOT NULL,
    "EntityId"        TEXT NOT NULL,
    "CreatedAt"       TIMESTAMP WITHOUT TIME ZONE NOT NULL,
    "CreatedByUserId" TEXT,
    "Scope"           TEXT,
    CONSTRAINT "UQ_TenantEntityBindings_Tenant_Entity" UNIQUE ("TenantId", "EntityType", "EntityId")
);
CREATE INDEX IF NOT EXISTS "IX_TenantEntityBindings_TenantId"
    ON "${TableScheme}"."TenantEntityBindings" ("TenantId");
CREATE INDEX IF NOT EXISTS "IX_TenantEntityBindings_Entity"
    ON "${TableScheme}"."TenantEntityBindings" ("EntityType", "EntityId");
