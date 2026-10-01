CREATE TABLE IF NOT EXISTS "${TableScheme}"."TenantStorage"
(
    "TenantId"      TEXT PRIMARY KEY,
    "StorageKind"   INTEGER NOT NULL DEFAULT 0,
    "Status"        INTEGER NOT NULL DEFAULT 0,
    "SchemaName"    TEXT NOT NULL DEFAULT '',
    "ConnectionRef" TEXT NOT NULL DEFAULT '',
    "CreatedAt"     TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC'),
    "UpdatedAt"     TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC')
);
CREATE INDEX IF NOT EXISTS "IX_TenantStorage_StorageKind" ON "${TableScheme}"."TenantStorage" ("StorageKind");
CREATE INDEX IF NOT EXISTS "IX_TenantStorage_Status" ON "${TableScheme}"."TenantStorage" ("Status");
