CREATE TABLE IF NOT EXISTS "EntityReferences"
(
    "Id"          TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "EntityType"  TEXT NOT NULL,
    "EntityId"    TEXT NOT NULL,
    "DisplayName" TEXT NOT NULL DEFAULT '',
    "EntityUrl"   TEXT NOT NULL DEFAULT '',
    "CreatedAt"   TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
