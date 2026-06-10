CREATE TABLE IF NOT EXISTS "CrossTenantMessages" (
"Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
"SourceTenantId" UUID NOT NULL,
"TargetTenantId" UUID NOT NULL,
"RelationshipId" UUID NOT NULL,
"ContractId" UUID,
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
CONSTRAINT "FK_CrossTenantMessages_SourceTenantId" FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_CrossTenantMessages_TargetTenantId" FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_CrossTenantMessages_RelationshipId" FOREIGN KEY ("RelationshipId") REFERENCES "TenantRelationships" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_CrossTenantMessages_ContractId" FOREIGN KEY ("ContractId") REFERENCES "Contracts" ("Id") ON DELETE SET NULL
);

CREATE INDEX IF NOT EXISTS "IdxCrossTenantMessagesSourceTenantId" ON "CrossTenantMessages" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxCrossTenantMessagesTargetTenantId" ON "CrossTenantMessages" ("TargetTenantId");
CREATE INDEX IF NOT EXISTS "IdxCrossTenantMessagesRelationshipId" ON "CrossTenantMessages" ("RelationshipId");
CREATE INDEX IF NOT EXISTS "IdxCrossTenantMessagesContractId" ON "CrossTenantMessages" ("ContractId");
