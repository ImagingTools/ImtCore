INSERT INTO ${TableScheme}."TenantEntityBindings" ("Id", "TenantId", "EntityType", "EntityId", "CreatedAt")
SELECT gen_random_uuid()::text, "TenantId", '${TableName}', "DocumentId"::text, now()
FROM ${TableScheme}."${TableName}"
WHERE "TenantId" IS NOT NULL AND "TenantId" <> ''
ON CONFLICT ("TenantId", "EntityType", "EntityId") DO NOTHING;

ALTER TABLE ${TableScheme}."${TableName}" DROP COLUMN IF EXISTS "TenantId";
