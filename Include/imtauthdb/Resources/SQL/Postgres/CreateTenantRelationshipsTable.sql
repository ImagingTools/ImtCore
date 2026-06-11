CREATE TABLE IF NOT EXISTS "TenantConnectionCodes" (
"TenantId" UUID PRIMARY KEY,
"ConnectionCode" TEXT NOT NULL UNIQUE,
"AllowConnectionsByCode" BOOLEAN NOT NULL DEFAULT true,
"CreatedAt" TIMESTAMP NOT NULL,
CONSTRAINT "FK_TenantConnectionCodes_TenantId" FOREIGN KEY ("TenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxTenantConnectionCodesCode" ON "TenantConnectionCodes" ("ConnectionCode");

CREATE TABLE IF NOT EXISTS "TenantConnectionRequests" (
"Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
"SourceTenantId" UUID NOT NULL,
"TargetTenantId" UUID NOT NULL,
"ConnectionCode" TEXT,
"Message" TEXT,
"Status" INTEGER NOT NULL DEFAULT 0,
"CreatedAt" TIMESTAMP NOT NULL,
"RespondedAt" TIMESTAMP,
CONSTRAINT "FK_TenantConnectionRequests_SourceTenantId" FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_TenantConnectionRequests_TargetTenantId" FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxTenantConnectionRequestsSourceTenantId" ON "TenantConnectionRequests" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxTenantConnectionRequestsTargetTenantId" ON "TenantConnectionRequests" ("TargetTenantId");

CREATE TABLE IF NOT EXISTS "TenantConnections" (
"Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
"TenantAId" UUID NOT NULL,
"TenantBId" UUID NOT NULL,
"Status" INTEGER NOT NULL DEFAULT 0,
"CreatedAt" TIMESTAMP NOT NULL,
"UpdatedAt" TIMESTAMP,
UNIQUE ("TenantAId", "TenantBId"),
CONSTRAINT "FK_TenantConnections_TenantAId" FOREIGN KEY ("TenantAId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_TenantConnections_TenantBId" FOREIGN KEY ("TenantBId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxTenantConnectionsTenantAId" ON "TenantConnections" ("TenantAId");
CREATE INDEX IF NOT EXISTS "IdxTenantConnectionsTenantBId" ON "TenantConnections" ("TenantBId");

CREATE TABLE IF NOT EXISTS "TenantRelationships" (
"Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
"ConnectionId" UUID NOT NULL,
"SourceTenantId" UUID NOT NULL,
"TargetTenantId" UUID NOT NULL,
"SourceRole" INTEGER NOT NULL DEFAULT 2,
"TargetRole" INTEGER NOT NULL DEFAULT 2,
"Scope" TEXT,
"ValidFrom" TEXT,
"ValidUntil" TEXT,
"Status" INTEGER NOT NULL DEFAULT 0,
"Description" TEXT,
"CreatedAt" TIMESTAMP NOT NULL,
"UpdatedAt" TIMESTAMP,
CONSTRAINT "FK_TenantRelationships_ConnectionId" FOREIGN KEY ("ConnectionId") REFERENCES "TenantConnections" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_TenantRelationships_SourceTenantId" FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_TenantRelationships_TargetTenantId" FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipsConnectionId" ON "TenantRelationships" ("ConnectionId");
CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipsSourceTenantId" ON "TenantRelationships" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipsTargetTenantId" ON "TenantRelationships" ("TargetTenantId");

CREATE TABLE IF NOT EXISTS "TenantRelationshipProposals" (
"Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
"ConnectionId" UUID NOT NULL,
"ExistingRelationshipId" UUID,
"ProposalType" INTEGER NOT NULL DEFAULT 0,
"InitiatorTenantId" UUID NOT NULL,
"CounterpartyTenantId" UUID NOT NULL,
"ProposedSourceRole" INTEGER NOT NULL DEFAULT 2,
"ProposedTargetRole" INTEGER NOT NULL DEFAULT 2,
"ProposedScope" TEXT,
"ProposedDescription" TEXT,
"ProposedValidFrom" TEXT,
"ProposedValidUntil" TEXT,
"Status" INTEGER NOT NULL DEFAULT 0,
"Message" TEXT,
"CreatedAt" TIMESTAMP NOT NULL,
"UpdatedAt" TIMESTAMP,
CONSTRAINT "FK_TenantRelationshipProposals_ConnectionId" FOREIGN KEY ("ConnectionId") REFERENCES "TenantConnections" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_TenantRelationshipProposals_InitiatorTenantId" FOREIGN KEY ("InitiatorTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_TenantRelationshipProposals_CounterpartyTenantId" FOREIGN KEY ("CounterpartyTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_TenantRelationshipProposals_ExistingRelationshipId" FOREIGN KEY ("ExistingRelationshipId") REFERENCES "TenantRelationships" ("Id") ON DELETE SET NULL
);

CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipProposalsConnectionId" ON "TenantRelationshipProposals" ("ConnectionId");
CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipProposalsInitiator" ON "TenantRelationshipProposals" ("InitiatorTenantId");
CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipProposalsCounterparty" ON "TenantRelationshipProposals" ("CounterpartyTenantId");
