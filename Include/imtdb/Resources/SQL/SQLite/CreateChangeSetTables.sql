CREATE TABLE IF NOT EXISTS "ChangeSets" (
	"Id" TEXT PRIMARY KEY,
	"TenantId" TEXT NOT NULL,
	"OwnerId" TEXT,
	"Origin" TEXT,
	"Summary" TEXT,
	"ReferenceId" TEXT,
	"State" TEXT NOT NULL,
	"CreatedAt" TEXT NOT NULL
);

CREATE INDEX IF NOT EXISTS "IdxChangeSetsTenantCreatedAt" ON "ChangeSets" ("TenantId", "CreatedAt");

-- The removal of the expired change sets
CREATE INDEX IF NOT EXISTS "IdxChangeSetsCreatedAt" ON "ChangeSets" ("CreatedAt");

CREATE TABLE IF NOT EXISTS "ChangeSetEntries" (
	"Id" TEXT PRIMARY KEY,
	"ChangeSetId" TEXT NOT NULL,
	"Position" BIGINT NOT NULL,
	"TableSchema" TEXT,
	"TableName" TEXT NOT NULL,
	"DocumentId" TEXT,
	"Operation" TEXT NOT NULL,
	"RevisionBefore" BIGINT,
	"RevisionAfter" BIGINT
);

CREATE INDEX IF NOT EXISTS "IdxChangeSetEntriesChangeSetId" ON "ChangeSetEntries" ("ChangeSetId", "Position");
