CREATE TABLE IF NOT EXISTS "${TableScheme}"."Tenants"
(
    "Id"          UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "Name"        TEXT NOT NULL,
    "Description" TEXT NOT NULL DEFAULT '',
    "OwnerId"     TEXT NOT NULL DEFAULT '',
    "CreatorId"   TEXT NOT NULL DEFAULT '',
    "IsActive"    BOOLEAN NOT NULL DEFAULT TRUE,
    "CreatedAt"   TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC'),
    "UpdatedAt"   TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC')
);
CREATE INDEX IF NOT EXISTS "IX_Tenants_Name" ON "${TableScheme}"."Tenants" ("Name");
CREATE INDEX IF NOT EXISTS "IX_Tenants_OwnerId" ON "${TableScheme}"."Tenants" ("OwnerId");
CREATE INDEX IF NOT EXISTS "IX_Tenants_CreatorId" ON "${TableScheme}"."Tenants" ("CreatorId");
CREATE INDEX IF NOT EXISTS "IX_Tenants_IsActive" ON "${TableScheme}"."Tenants" ("IsActive");
