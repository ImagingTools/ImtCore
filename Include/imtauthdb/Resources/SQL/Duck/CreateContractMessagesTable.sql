CREATE TABLE IF NOT EXISTS "ContractMessages" (
"Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
"SourceTenantId" UUID NOT NULL,
"TargetTenantId" UUID NOT NULL,
"LinkId" UUID NOT NULL,
"ContractId" UUID NOT NULL,
"SourceObjectId" TEXT,
"TargetObjectId" TEXT,
"MessageType" INTEGER NOT NULL,
"CustomType" TEXT,
"Payload" TEXT,
"Status" INTEGER NOT NULL,
"ErrorMessage" TEXT,
"CreatedAt" TIMESTAMP NOT NULL,
"UpdatedAt" TIMESTAMP NOT NULL,
"ExpiresAt" TIMESTAMP,
CONSTRAINT "FK_ContractMessages_SourceTenantId" FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_ContractMessages_TargetTenantId" FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxContractMessagesSourceTenantId" ON "ContractMessages" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxContractMessagesTargetTenantId" ON "ContractMessages" ("TargetTenantId");
CREATE INDEX IF NOT EXISTS "IdxContractMessagesLinkId" ON "ContractMessages" ("LinkId");
CREATE INDEX IF NOT EXISTS "IdxContractMessagesContractId" ON "ContractMessages" ("ContractId");
