CREATE TABLE IF NOT EXISTS "Contracts" (
	"Id" TEXT PRIMARY KEY,
	"RelationshipId" TEXT NOT NULL,
	"SourceTenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT NOT NULL,
	"Status" INTEGER NOT NULL,
	"Scope" TEXT,
	"ValidFrom" TIMESTAMP,
	"ValidUntil" TIMESTAMP,
	"Description" TEXT,
	"Terms" TEXT,
	"CreatedAt" TIMESTAMP NOT NULL,
	"UpdatedAt" TIMESTAMP NOT NULL,
	CONSTRAINT "FK_Contracts_SourceTenantId" FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
	CONSTRAINT "FK_Contracts_TargetTenantId" FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
	CONSTRAINT "FK_Contracts_RelationshipId" FOREIGN KEY ("RelationshipId") REFERENCES "TenantRelationships" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxContractsRelationshipId" ON "Contracts" ("RelationshipId");
CREATE INDEX IF NOT EXISTS "IdxContractsSourceTenantId" ON "Contracts" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxContractsTargetTenantId" ON "Contracts" ("TargetTenantId");
