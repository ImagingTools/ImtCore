CREATE TABLE IF NOT EXISTS "TenantEntityBindings"
(
    "Id"              TEXT PRIMARY KEY,
    "TenantId"        TEXT NOT NULL,
    "EntityType"      TEXT NOT NULL,
    "EntityId"        TEXT NOT NULL,
    "CreatedAt"       TEXT NOT NULL,
    "CreatedByUserId" TEXT,
    UNIQUE ("TenantId", "EntityType", "EntityId")
);
