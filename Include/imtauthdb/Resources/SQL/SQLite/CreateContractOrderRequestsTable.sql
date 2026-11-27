CREATE TABLE IF NOT EXISTS "ContractOrderRequests" (
	"Id" TEXT PRIMARY KEY,
	"MessageId" TEXT,
	"SourceTenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT NOT NULL,
	"LinkId" TEXT NOT NULL,
	"ContractId" TEXT NOT NULL,
	"SourceOrderId" TEXT,
	"ArticleNumber" TEXT,
	"Quantity" INTEGER NOT NULL DEFAULT 0,
	"Note" TEXT,
	"Status" INTEGER NOT NULL,
	"StatusNote" TEXT,
	"CreatedAt" TEXT NOT NULL,
	"UpdatedAt" TEXT NOT NULL,
	FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
	FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxContractOrderRequestsSourceTenantId" ON "ContractOrderRequests" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxContractOrderRequestsTargetTenantId" ON "ContractOrderRequests" ("TargetTenantId");
CREATE INDEX IF NOT EXISTS "IdxContractOrderRequestsLinkId" ON "ContractOrderRequests" ("LinkId");
CREATE INDEX IF NOT EXISTS "IdxContractOrderRequestsContractId" ON "ContractOrderRequests" ("ContractId");
