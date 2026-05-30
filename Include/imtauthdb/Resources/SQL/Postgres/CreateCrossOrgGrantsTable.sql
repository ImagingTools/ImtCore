CREATE TABLE IF NOT EXISTS "CrossOrgGrants" (
	"Id" TEXT PRIMARY KEY,
	"SourceTenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT NOT NULL,
	"RelationshipId" TEXT NOT NULL,
	"ContractId" TEXT,
	"TargetTeamId" TEXT,
	"AccessLevel" INTEGER NOT NULL,
	"ResourceScope" TEXT,
	"Description" TEXT,
	"CreatedAt" TIMESTAMP NOT NULL,
	"ExpiresAt" TIMESTAMP,
	"IsActive" INTEGER NOT NULL DEFAULT 1
);

CREATE INDEX IF NOT EXISTS "IdxCrossOrgGrantsSourceTenantId" ON "CrossOrgGrants" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxCrossOrgGrantsTargetTenantId" ON "CrossOrgGrants" ("TargetTenantId");
CREATE INDEX IF NOT EXISTS "IdxCrossOrgGrantsRelationshipId" ON "CrossOrgGrants" ("RelationshipId");
CREATE INDEX IF NOT EXISTS "IdxCrossOrgGrantsContractId" ON "CrossOrgGrants" ("ContractId");
