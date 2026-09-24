CREATE TABLE IF NOT EXISTS "TagEvents"
(
    "Id"             TEXT PRIMARY KEY,
    "Action"         TEXT NOT NULL,
    "TagId"          TEXT NOT NULL,
    "TagName"        TEXT,
    "TagColor"       TEXT,
    "EntityType"     TEXT NOT NULL,
    "EntityId"       TEXT NOT NULL,
    "EntityRevision" INTEGER,
    "ActorId"        TEXT,
    "ActorName"      TEXT,
    "TenantId"       TEXT,
    "Timestamp"      TEXT NOT NULL
);
CREATE INDEX IF NOT EXISTS "IX_TagEvents_Entity_Timestamp" ON "TagEvents" ("EntityType", "EntityId", "Timestamp");
