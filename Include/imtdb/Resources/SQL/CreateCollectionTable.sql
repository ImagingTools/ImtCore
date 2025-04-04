DO $$ 
BEGIN
   IF NOT EXISTS (SELECT 1 FROM pg_type WHERE typname = 'DocumentState') THEN
      CREATE TYPE "DocumentState" AS ENUM ('Active', 'InActive', 'Disabled');
   END IF;
END $$;

CREATE TABLE IF NOT EXISTS "${TableName}"
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

CREATE INDEX IF NOT EXISTS "${TableName}DocumentIdIndex"
    ON public."${TableName}" USING btree
    ("DocumentId" ASC NULLS LAST)
    WITH (deduplicate_items=True)
    TABLESPACE pg_default;

CREATE INDEX IF NOT EXISTS "${TableName}StateIndex"
    ON public."${TableName}" USING btree
    ("State" ASC NULLS LAST)
    TABLESPACE pg_default;

CREATE INDEX IF NOT EXISTS "${TableName}RevisionNumberIndex"
    ON public."${TableName}" ((("RevisionInfo"->>'RevisionNumber')::int));
