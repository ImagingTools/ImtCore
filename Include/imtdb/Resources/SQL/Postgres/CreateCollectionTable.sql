DO $$ 
BEGIN
   IF NOT EXISTS (SELECT 1 FROM pg_type WHERE typname = 'DocumentState') THEN
      CREATE TYPE "DocumentState" AS ENUM ('Active', 'InActive', 'Disabled');
   END IF;
END $$;

CREATE TABLE IF NOT EXISTS ${TableScheme}."${TableName}"
(
    "Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "DocumentId" UUID NOT NULL,
    "TypeId" Text, -- optional !
    "Document" jsonb NOT NULL,
    "Name" Text, -- optional !
    "Description" Text, -- optional !
    "RevisionInfo" jsonb, -- OwnerId, OwnerName, OperationDesc, RevisionNumber, Checksum
    "DataMetaInfo" jsonb,
    "Derivates" jsonb,
    "TimeStamp" timestamp without time zone  NOT NULL, -- UTC Time
    "State" "DocumentState"
);

-- Shared tenant scope table; kept idempotent here to avoid a separate migration/init component.
CREATE TABLE IF NOT EXISTS ${TableScheme}."TenantEntityBindings"
(
    "Id" TEXT PRIMARY KEY,
    "TenantId" TEXT NOT NULL,
    "EntityType" TEXT NOT NULL,
    "EntityId" TEXT NOT NULL,
    "CreatedAt" timestamp without time zone NOT NULL,
    "CreatedByUserId" TEXT,
    "Scope" TEXT,
    CONSTRAINT "UQ_TenantEntityBindings_Tenant_Entity" UNIQUE ("TenantId", "EntityType", "EntityId")
);

CREATE INDEX IF NOT EXISTS "IX_TenantEntityBindings_TenantId"
    ON ${TableScheme}."TenantEntityBindings" ("TenantId");

CREATE INDEX IF NOT EXISTS "IX_TenantEntityBindings_Entity"
    ON ${TableScheme}."TenantEntityBindings" ("EntityType", "EntityId");

CREATE INDEX IF NOT EXISTS "${TableName}DocumentIdIndex"
    ON ${TableScheme}."${TableName}" USING btree
    ("DocumentId" ASC NULLS LAST)
    TABLESPACE pg_default;

CREATE INDEX IF NOT EXISTS "${TableName}StateIndex"
    ON ${TableScheme}."${TableName}" USING btree
    ("State" ASC NULLS LAST)
    TABLESPACE pg_default;

CREATE INDEX IF NOT EXISTS "${TableName}RevisionNumberIndex"
    ON ${TableScheme}."${TableName}" ((("RevisionInfo"->>'RevisionNumber')::int));
