CREATE TABLE IF NOT EXISTS "CrossTenantMessages" (
	"Id" TEXT PRIMARY KEY,
	"SourceTenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT NOT NULL,
	"RelationshipId" TEXT NOT NULL,
	"ContractId" TEXT,
	"SourceObjectId" TEXT,
	"TargetObjectId" TEXT,
	"MessageType" INTEGER NOT NULL,
	"CustomType" TEXT,
	"Payload" TEXT,
	"Status" INTEGER NOT NULL,
	"ErrorMessage" TEXT,
	"CreatedAt" TIMESTAMP NOT NULL,
	"UpdatedAt" TIMESTAMP NOT NULL,
	"ExpiresAt" TIMESTAMP
);

CREATE INDEX IF NOT EXISTS "IdxCrossTenantMessagesSourceTenantId" ON "CrossTenantMessages" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxCrossTenantMessagesTargetTenantId" ON "CrossTenantMessages" ("TargetTenantId");
CREATE INDEX IF NOT EXISTS "IdxCrossTenantMessagesRelationshipId" ON "CrossTenantMessages" ("RelationshipId");
CREATE INDEX IF NOT EXISTS "IdxCrossTenantMessagesContractId" ON "CrossTenantMessages" ("ContractId");
