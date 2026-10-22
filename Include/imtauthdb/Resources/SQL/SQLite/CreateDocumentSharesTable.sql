CREATE TABLE IF NOT EXISTS "DocumentShares"
(
    "Id"              TEXT PRIMARY KEY,
    "CollectionId"    TEXT NOT NULL,
    "EntityId"        TEXT NOT NULL,
    "EntityName"      TEXT,
    "OwnerTenantId"   TEXT NOT NULL,
    "OwnerTenantName" TEXT,
    "TargetTenantId"  TEXT NOT NULL,
    "TargetTenantName" TEXT,
    "TargetGroupIds"  TEXT NOT NULL DEFAULT '',
    "AccessLevel"     INTEGER NOT NULL,
    "Status"          INTEGER NOT NULL,
    "ValidUntil"      TEXT,
    "Message"         TEXT,
    "CreatedByUserId" TEXT,
    "CreatedAt"       TEXT NOT NULL,
    "UpdatedAt"       TEXT,
    UNIQUE ("EntityId", "TargetTenantId")
);
CREATE INDEX IF NOT EXISTS "IX_DocumentShares_TargetTenantId_Status" ON "DocumentShares" ("TargetTenantId", "Status");
CREATE INDEX IF NOT EXISTS "IX_DocumentShares_OwnerTenantId" ON "DocumentShares" ("OwnerTenantId");
