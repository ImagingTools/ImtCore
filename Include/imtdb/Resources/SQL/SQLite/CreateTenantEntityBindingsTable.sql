CREATE TABLE IF NOT EXISTS "TenantEntityBindings"
(
    "Id" TEXT PRIMARY KEY,
    "TenantId" TEXT NOT NULL,
    "EntityType" TEXT NOT NULL,
    "EntityId" TEXT NOT NULL,
    "CreatedAt" TEXT NOT NULL,
    "CreatedByUserId" TEXT,
    "Scope" TEXT,
    UNIQUE ("TenantId", "EntityType", "EntityId")
);

CREATE INDEX IF NOT EXISTS "IX_TenantEntityBindings_TenantId"
    ON "TenantEntityBindings" ("TenantId");

CREATE INDEX IF NOT EXISTS "IX_TenantEntityBindings_Entity"
    ON "TenantEntityBindings" ("EntityType", "EntityId");
