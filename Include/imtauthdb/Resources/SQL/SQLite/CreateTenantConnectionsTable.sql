CREATE TABLE IF NOT EXISTS "TenantConnectionCodes" (
	"TenantId" TEXT PRIMARY KEY,
	"ConnectionCode" TEXT NOT NULL UNIQUE,
	"AllowConnectionsByCode" INTEGER NOT NULL DEFAULT 1,
	"CreatedAt" TEXT NOT NULL,
	FOREIGN KEY ("TenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxTenantConnectionCodesCode" ON "TenantConnectionCodes" ("ConnectionCode");

CREATE TABLE IF NOT EXISTS "TenantConnectionRequests" (
	"Id" TEXT PRIMARY KEY,
	"SourceTenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT NOT NULL,
	"ConnectionCode" TEXT,
	"Message" TEXT,
	"Status" INTEGER NOT NULL DEFAULT 0,
	"CreatedAt" TEXT NOT NULL,
	"RespondedAt" TEXT,
	FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
	FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxTenantConnectionRequestsSourceTenantId" ON "TenantConnectionRequests" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxTenantConnectionRequestsTargetTenantId" ON "TenantConnectionRequests" ("TargetTenantId");

CREATE TABLE IF NOT EXISTS "TenantConnections" (
	"Id" TEXT PRIMARY KEY,
	"TenantAId" TEXT NOT NULL,
	"TenantBId" TEXT NOT NULL,
	"Status" INTEGER NOT NULL DEFAULT 0,
	"CreatedAt" TEXT NOT NULL,
	"UpdatedAt" TEXT,
	UNIQUE ("TenantAId", "TenantBId"),
	FOREIGN KEY ("TenantAId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
	FOREIGN KEY ("TenantBId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxTenantConnectionsTenantAId" ON "TenantConnections" ("TenantAId");
CREATE INDEX IF NOT EXISTS "IdxTenantConnectionsTenantBId" ON "TenantConnections" ("TenantBId");
