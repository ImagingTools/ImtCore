CREATE TABLE IF NOT EXISTS "TenantConnectionRequests" (
	"Id" TEXT PRIMARY KEY,
	"SourceTenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT,
	"TargetIdentifier" TEXT,
	"ConnectCode" TEXT,
	"ProposedSourceRole" INTEGER NOT NULL,
	"ProposedTargetRole" INTEGER NOT NULL,
	"Message" TEXT,
	"Status" INTEGER NOT NULL DEFAULT 0,
	"CreatedAt" TEXT NOT NULL,
	"ExpiresAt" TEXT,
	"RespondedAt" TEXT
);

CREATE INDEX IF NOT EXISTS "IdxTenantConnectionRequestsSourceTenantId" ON "TenantConnectionRequests" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxTenantConnectionRequestsTargetTenantId" ON "TenantConnectionRequests" ("TargetTenantId");
CREATE INDEX IF NOT EXISTS "IdxTenantConnectionRequestsConnectCode" ON "TenantConnectionRequests" ("ConnectCode");
