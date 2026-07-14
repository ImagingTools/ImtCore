CREATE TABLE IF NOT EXISTS "Contracts" (
	"Id" TEXT PRIMARY KEY,
	"RelationshipId" TEXT NOT NULL,
	"SourceTenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT NOT NULL,
	"Status" INTEGER NOT NULL,
	"Scope" TEXT,
	"ValidFrom" TEXT,
	"ValidUntil" TEXT,
	"Description" TEXT,
	"Terms" TEXT,
	"CreatedAt" TEXT NOT NULL,
	"UpdatedAt" TEXT NOT NULL,
	FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
	FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
	FOREIGN KEY ("RelationshipId") REFERENCES "TenantRelationships" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxContractsRelationshipId" ON "Contracts" ("RelationshipId");
CREATE INDEX IF NOT EXISTS "IdxContractsSourceTenantId" ON "Contracts" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxContractsTargetTenantId" ON "Contracts" ("TargetTenantId");
