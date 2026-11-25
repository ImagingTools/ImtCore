CREATE TABLE IF NOT EXISTS "ContractMessages" (
	"Id" TEXT PRIMARY KEY,
	"SourceTenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT NOT NULL,
	"LinkId" TEXT NOT NULL,
	"ContractId" TEXT NOT NULL,
	"SourceObjectId" TEXT,
	"TargetObjectId" TEXT,
	"MessageType" INTEGER NOT NULL,
	"CustomType" TEXT,
	"Payload" TEXT,
	"Status" INTEGER NOT NULL,
	"ErrorMessage" TEXT,
	"CreatedAt" TEXT NOT NULL,
	"UpdatedAt" TEXT NOT NULL,
	"ExpiresAt" TEXT,
	FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
	FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxContractMessagesSourceTenantId" ON "ContractMessages" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxContractMessagesTargetTenantId" ON "ContractMessages" ("TargetTenantId");
CREATE INDEX IF NOT EXISTS "IdxContractMessagesLinkId" ON "ContractMessages" ("LinkId");
CREATE INDEX IF NOT EXISTS "IdxContractMessagesContractId" ON "ContractMessages" ("ContractId");
