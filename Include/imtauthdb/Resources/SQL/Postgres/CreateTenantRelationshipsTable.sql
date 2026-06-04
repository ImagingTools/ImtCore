CREATE TABLE IF NOT EXISTS "TenantConnectionCodes" (
	"TenantId" TEXT PRIMARY KEY,
	"ConnectionCode" TEXT NOT NULL UNIQUE,
	"AllowConnectionsByCode" BOOLEAN NOT NULL DEFAULT true,
	"CreatedAt" TIMESTAMP NOT NULL
);

CREATE INDEX IF NOT EXISTS "IdxTenantConnectionCodesCode" ON "TenantConnectionCodes" ("ConnectionCode");

CREATE TABLE IF NOT EXISTS "TenantConnectionRequests" (
	"Id" TEXT PRIMARY KEY,
	"SourceTenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT NOT NULL,
	"ConnectionCode" TEXT,
	"Message" TEXT,
	"Status" INTEGER NOT NULL DEFAULT 0,
	"CreatedAt" TIMESTAMP NOT NULL,
	"RespondedAt" TIMESTAMP
);

CREATE INDEX IF NOT EXISTS "IdxTenantConnectionRequestsSourceTenantId" ON "TenantConnectionRequests" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxTenantConnectionRequestsTargetTenantId" ON "TenantConnectionRequests" ("TargetTenantId");

CREATE TABLE IF NOT EXISTS "TenantConnections" (
	"Id" TEXT PRIMARY KEY,
	"TenantAId" TEXT NOT NULL,
	"TenantBId" TEXT NOT NULL,
	"Status" INTEGER NOT NULL DEFAULT 0,
	"CreatedAt" TIMESTAMP NOT NULL,
	"UpdatedAt" TIMESTAMP,
	UNIQUE ("TenantAId", "TenantBId")
);

CREATE INDEX IF NOT EXISTS "IdxTenantConnectionsTenantAId" ON "TenantConnections" ("TenantAId");
CREATE INDEX IF NOT EXISTS "IdxTenantConnectionsTenantBId" ON "TenantConnections" ("TenantBId");

CREATE TABLE IF NOT EXISTS "TenantRelationships" (
	"Id" TEXT PRIMARY KEY,
	"ConnectionId" TEXT NOT NULL,
	"SourceTenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT NOT NULL,
	"SourceRole" INTEGER NOT NULL DEFAULT 2,
	"TargetRole" INTEGER NOT NULL DEFAULT 2,
	"Scope" TEXT,
	"ValidFrom" TEXT,
	"ValidUntil" TEXT,
	"Status" INTEGER NOT NULL DEFAULT 0,
	"Description" TEXT,
	"CreatedAt" TIMESTAMP NOT NULL,
	"UpdatedAt" TIMESTAMP
);

CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipsConnectionId" ON "TenantRelationships" ("ConnectionId");
CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipsSourceTenantId" ON "TenantRelationships" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipsTargetTenantId" ON "TenantRelationships" ("TargetTenantId");

CREATE TABLE IF NOT EXISTS "TenantRelationshipProposals" (
	"Id" TEXT PRIMARY KEY,
	"ConnectionId" TEXT NOT NULL,
	"ExistingRelationshipId" TEXT,
	"ProposalType" INTEGER NOT NULL DEFAULT 0,
	"InitiatorTenantId" TEXT NOT NULL,
	"CounterpartyTenantId" TEXT NOT NULL,
	"ProposedSourceRole" INTEGER NOT NULL DEFAULT 2,
	"ProposedTargetRole" INTEGER NOT NULL DEFAULT 2,
	"ProposedScope" TEXT,
	"ProposedDescription" TEXT,
	"ProposedValidFrom" TEXT,
	"ProposedValidUntil" TEXT,
	"Status" INTEGER NOT NULL DEFAULT 0,
	"Message" TEXT,
	"CreatedAt" TIMESTAMP NOT NULL,
	"UpdatedAt" TIMESTAMP
);

CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipProposalsConnectionId" ON "TenantRelationshipProposals" ("ConnectionId");
CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipProposalsInitiator" ON "TenantRelationshipProposals" ("InitiatorTenantId");
CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipProposalsCounterparty" ON "TenantRelationshipProposals" ("CounterpartyTenantId");
