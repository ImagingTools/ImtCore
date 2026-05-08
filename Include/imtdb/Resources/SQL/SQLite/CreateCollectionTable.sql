CREATE TABLE IF NOT EXISTS "${TableName}"
(
    "Id" TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "DocumentId" TEXT NOT NULL,
    "TypeId" TEXT, -- optional !
    "Document" TEXT NOT NULL,
    "Name" TEXT, -- optional !
    "Description" TEXT, -- optional !
    "RevisionInfo" TEXT, -- OwnerId, OwnerName, OperationDesc, RevisionNumber, Checksum
    "DataMetaInfo" TEXT,
    "Derivates" TEXT,
    "TimeStamp" TEXT NOT NULL, -- UTC Time
    "State" TEXT
);

-- Shared tenant scope table; kept idempotent here to avoid a separate migration/init component.
CREATE TABLE IF NOT EXISTS "TenantEntityBindings"
(
    "Id" TEXT PRIMARY KEY,
    "TenantId" TEXT NOT NULL,
    "EntityType" TEXT NOT NULL,
    "EntityId" TEXT NOT NULL,
    "CreatedAt" TEXT NOT NULL,
    "CreatedByUserId" TEXT,
    "Scope" TEXT,
    UNIQUE ("TenantId", "EntityType", "EntityId")
);

CREATE INDEX IF NOT EXISTS "IX_TenantEntityBindings_TenantId"
    ON "TenantEntityBindings" ("TenantId");

CREATE INDEX IF NOT EXISTS "IX_TenantEntityBindings_Entity"
    ON "TenantEntityBindings" ("EntityType", "EntityId");
