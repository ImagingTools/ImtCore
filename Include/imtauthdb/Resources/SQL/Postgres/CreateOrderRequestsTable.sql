CREATE TABLE IF NOT EXISTS "OrderRequests" (
	"Id" TEXT PRIMARY KEY,
	"MessageId" TEXT,
	"SourceTenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT NOT NULL,
	"RelationshipId" TEXT NOT NULL,
	"ContractId" TEXT,
	"SourceOrderId" TEXT,
	"ArticleNumber" TEXT,
	"Quantity" INTEGER NOT NULL DEFAULT 0,
	"Note" TEXT,
	"Status" INTEGER NOT NULL,
	"StatusNote" TEXT,
	"CreatedAt" TIMESTAMP NOT NULL,
	"UpdatedAt" TIMESTAMP NOT NULL,
	CONSTRAINT "FK_OrderRequests_RelationshipId" FOREIGN KEY ("RelationshipId") REFERENCES "TenantRelationships" ("Id") ON DELETE CASCADE,
	CONSTRAINT "FK_OrderRequests_ContractId" FOREIGN KEY ("ContractId") REFERENCES "Contracts" ("Id") ON DELETE SET NULL
);

CREATE INDEX IF NOT EXISTS "IdxOrderRequestsSourceTenantId" ON "OrderRequests" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxOrderRequestsTargetTenantId" ON "OrderRequests" ("TargetTenantId");
CREATE INDEX IF NOT EXISTS "IdxOrderRequestsRelationshipId" ON "OrderRequests" ("RelationshipId");
CREATE INDEX IF NOT EXISTS "IdxOrderRequestsContractId" ON "OrderRequests" ("ContractId");
