CREATE TABLE IF NOT EXISTS "CrossOrgGrants" (
"Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
"SourceTenantId" UUID NOT NULL,
"TargetTenantId" UUID NOT NULL,
"Name" TEXT,
"RoleIds" TEXT,
"Description" TEXT,
"CreatedAt" TIMESTAMP NOT NULL,
"ExpiresAt" TIMESTAMP,
"IsActive" INTEGER NOT NULL DEFAULT 1,
CONSTRAINT "FK_CrossOrgGrants_SourceTenantId" FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_CrossOrgGrants_TargetTenantId" FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxCrossOrgGrantsSourceTenantId" ON "CrossOrgGrants" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxCrossOrgGrantsTargetTenantId" ON "CrossOrgGrants" ("TargetTenantId");
