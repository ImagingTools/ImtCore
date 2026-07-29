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
	"CreatedAt" TEXT NOT NULL,
	"UpdatedAt" TEXT NOT NULL,
	FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
	FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
	FOREIGN KEY ("RelationshipId") REFERENCES "TenantRelationships" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxOrderRequestsSourceTenantId" ON "OrderRequests" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxOrderRequestsTargetTenantId" ON "OrderRequests" ("TargetTenantId");
CREATE INDEX IF NOT EXISTS "IdxOrderRequestsRelationshipId" ON "OrderRequests" ("RelationshipId");
CREATE INDEX IF NOT EXISTS "IdxOrderRequestsContractId" ON "OrderRequests" ("ContractId");
