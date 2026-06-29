CREATE TABLE IF NOT EXISTS "TenantEntityBindings"
(
    "Id"              TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "TenantId"        TEXT NOT NULL,
    "EntityType"      TEXT NOT NULL,
    "EntityId"        TEXT NOT NULL,
    "CreatedAt"       TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "CreatedByUserId" TEXT,
    UNIQUE ("TenantId", "EntityType", "EntityId")
);
