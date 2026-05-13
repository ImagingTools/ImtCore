CREATE TABLE IF NOT EXISTS "${TableScheme}"."EntityReferences"
(
    "Id"          UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "EntityType"  TEXT NOT NULL,
    "EntityId"    TEXT NOT NULL,
    "DisplayName" TEXT NOT NULL DEFAULT '',
    "EntityUrl"   TEXT NOT NULL DEFAULT '',
    "CreatedAt"   TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC')
);
