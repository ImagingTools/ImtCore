CREATE TABLE IF NOT EXISTS "OrderRequests" (
"Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
"MessageId" UUID,
"SourceTenantId" UUID NOT NULL,
"TargetTenantId" UUID NOT NULL,
"RelationshipId" UUID NOT NULL,
"ContractId" UUID,
"SourceOrderId" TEXT,
"ArticleNumber" TEXT,
"Quantity" INTEGER NOT NULL DEFAULT 0,
"Note" TEXT,
"Status" INTEGER NOT NULL,
"StatusNote" TEXT,
"CreatedAt" TIMESTAMP NOT NULL,
"UpdatedAt" TIMESTAMP NOT NULL,
CONSTRAINT "FK_OrderRequests_SourceTenantId" FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_OrderRequests_TargetTenantId" FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_OrderRequests_RelationshipId" FOREIGN KEY ("RelationshipId") REFERENCES "TenantRelationships" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxOrderRequestsSourceTenantId" ON "OrderRequests" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxOrderRequestsTargetTenantId" ON "OrderRequests" ("TargetTenantId");
CREATE INDEX IF NOT EXISTS "IdxOrderRequestsRelationshipId" ON "OrderRequests" ("RelationshipId");
CREATE INDEX IF NOT EXISTS "IdxOrderRequestsContractId" ON "OrderRequests" ("ContractId");
