CREATE TABLE IF NOT EXISTS "TenantStorage"
(
    "TenantId"      TEXT PRIMARY KEY,
    "StorageKind"   INTEGER NOT NULL DEFAULT 0,
    "Status"        INTEGER NOT NULL DEFAULT 0,
    "SchemaName"    TEXT NOT NULL DEFAULT '',
    "ConnectionRef" TEXT NOT NULL DEFAULT '',
    "CreatedAt"     TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "UpdatedAt"     TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX IF NOT EXISTS "IdxTenantStorageStorageKind" ON "TenantStorage" ("StorageKind");
CREATE INDEX IF NOT EXISTS "IdxTenantStorageStatus" ON "TenantStorage" ("Status");
