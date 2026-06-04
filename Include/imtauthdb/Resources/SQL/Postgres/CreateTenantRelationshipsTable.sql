CREATE TABLE IF NOT EXISTS "TenantRelationships" (
	"Id" TEXT PRIMARY KEY,
	"TenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT NOT NULL,
	"Role" INTEGER NOT NULL DEFAULT 2,
	"SourceRole" INTEGER NOT NULL DEFAULT 2,
	"TargetRole" INTEGER NOT NULL DEFAULT 2,
	"Scope" TEXT,
	"ValidFrom" TEXT,
	"ValidUntil" TEXT,
	"IsActive" BOOLEAN NOT NULL DEFAULT true,
	"Description" TEXT,
	"CreatedAt" TIMESTAMP NOT NULL
);

CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipsTenantId" ON "TenantRelationships" ("TenantId");
CREATE INDEX IF NOT EXISTS "IdxTenantRelationshipsTargetTenantId" ON "TenantRelationships" ("TargetTenantId");
