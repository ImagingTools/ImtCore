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
