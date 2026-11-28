CREATE TABLE IF NOT EXISTS "DocumentActions" (
	"Id" TEXT PRIMARY KEY,
	"TenantId" TEXT NOT NULL,
	"OwnerId" TEXT,
	"Origin" TEXT,
	"Summary" TEXT,
	"ReferenceId" TEXT,
	"State" TEXT NOT NULL,
	"CreatedAt" TEXT NOT NULL
);

CREATE INDEX IF NOT EXISTS "IdxDocumentActionsTenantCreatedAt" ON "DocumentActions" ("TenantId", "CreatedAt");

-- The removal of the expired document actions
CREATE INDEX IF NOT EXISTS "IdxDocumentActionsCreatedAt" ON "DocumentActions" ("CreatedAt");

CREATE TABLE IF NOT EXISTS "DocumentActionEntries" (
	"Id" TEXT PRIMARY KEY,
	"DocumentActionId" TEXT NOT NULL,
	"Position" BIGINT NOT NULL,
	"TableSchema" TEXT,
	"TableName" TEXT NOT NULL,
	"DocumentId" TEXT,
	"Operation" TEXT NOT NULL,
	"RevisionBefore" BIGINT,
	"RevisionAfter" BIGINT
);

CREATE INDEX IF NOT EXISTS "IdxDocumentActionEntriesDocumentActionId" ON "DocumentActionEntries" ("DocumentActionId", "Position");
