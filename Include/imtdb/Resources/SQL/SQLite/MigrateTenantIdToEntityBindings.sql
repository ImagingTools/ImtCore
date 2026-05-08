INSERT OR IGNORE INTO "TenantEntityBindings" ("Id", "TenantId", "EntityType", "EntityId", "CreatedAt")
SELECT lower(hex(randomblob(16))), "TenantId", '${TableName}', "DocumentId", datetime('now')
FROM "${TableName}"
WHERE "TenantId" IS NOT NULL AND "TenantId" <> '';

ALTER TABLE "${TableName}" DROP COLUMN "TenantId";
