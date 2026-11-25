CREATE TABLE IF NOT EXISTS "ContractOrderRequests" (
"Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
"MessageId" UUID,
"SourceTenantId" UUID NOT NULL,
"TargetTenantId" UUID NOT NULL,
"LinkId" UUID NOT NULL,
"ContractId" UUID NOT NULL,
"SourceOrderId" TEXT,
"ArticleNumber" TEXT,
"Quantity" INTEGER NOT NULL DEFAULT 0,
"Note" TEXT,
"Status" INTEGER NOT NULL,
"StatusNote" TEXT,
"CreatedAt" TIMESTAMP NOT NULL,
"UpdatedAt" TIMESTAMP NOT NULL,
CONSTRAINT "FK_ContractOrderRequests_SourceTenantId" FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_ContractOrderRequests_TargetTenantId" FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxContractOrderRequestsSourceTenantId" ON "ContractOrderRequests" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxContractOrderRequestsTargetTenantId" ON "ContractOrderRequests" ("TargetTenantId");
CREATE INDEX IF NOT EXISTS "IdxContractOrderRequestsLinkId" ON "ContractOrderRequests" ("LinkId");
CREATE INDEX IF NOT EXISTS "IdxContractOrderRequestsContractId" ON "ContractOrderRequests" ("ContractId");
