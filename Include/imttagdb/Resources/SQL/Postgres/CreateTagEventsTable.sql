CREATE TABLE IF NOT EXISTS "${TableScheme}"."TagEvents"
(
    "Id"             TEXT PRIMARY KEY,
    "Action"         TEXT NOT NULL,
    "TagId"          TEXT NOT NULL,
    "TagName"        TEXT,
    "TagColor"       TEXT,
    "EntityType"     TEXT NOT NULL,
    "EntityId"       TEXT NOT NULL,
    "EntityRevision" BIGINT,
    "ActorId"        TEXT,
    "ActorName"      TEXT,
    "TenantId"       TEXT,
    "Timestamp"      TIMESTAMP WITHOUT TIME ZONE NOT NULL
);
CREATE INDEX IF NOT EXISTS "IX_TagEvents_Entity_Timestamp" ON "${TableScheme}"."TagEvents" ("EntityType", "EntityId", "Timestamp");
