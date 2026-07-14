CREATE TABLE IF NOT EXISTS "CrossOrgGrants" (
	"Id" TEXT PRIMARY KEY,
	"SourceTenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT NOT NULL,
	"Name" TEXT,
	"RoleIds" TEXT,
	"Description" TEXT,
	"CreatedAt" TEXT NOT NULL,
	"ExpiresAt" TEXT,
	"IsActive" INTEGER NOT NULL DEFAULT 1,
	FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
	FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxCrossOrgGrantsSourceTenantId" ON "CrossOrgGrants" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxCrossOrgGrantsTargetTenantId" ON "CrossOrgGrants" ("TargetTenantId");
