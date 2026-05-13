CREATE TABLE IF NOT EXISTS "Tenants"
(
    "Id"          TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "Name"        TEXT NOT NULL,
    "Description" TEXT NOT NULL DEFAULT '',
    "OwnerId"     TEXT NOT NULL DEFAULT '',
    "IsActive"    INTEGER NOT NULL DEFAULT 1,
    "CreatedAt"   TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "UpdatedAt"   TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX IF NOT EXISTS "IX_Tenants_Name" ON "Tenants" ("Name");
CREATE INDEX IF NOT EXISTS "IX_Tenants_OwnerId" ON "Tenants" ("OwnerId");
CREATE INDEX IF NOT EXISTS "IX_Tenants_IsActive" ON "Tenants" ("IsActive");
