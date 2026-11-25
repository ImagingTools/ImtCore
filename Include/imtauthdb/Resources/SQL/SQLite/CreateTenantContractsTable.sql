CREATE TABLE IF NOT EXISTS "TenantContracts" (
	"Id" TEXT PRIMARY KEY,
	"InitiatorTenantId" TEXT NOT NULL,
	"Number" TEXT,
	"Name" TEXT,
	"State" INTEGER NOT NULL DEFAULT 0,
	"SubmittedAt" TEXT,
	"CreatedAt" TEXT,
	"UpdatedAt" TEXT,
	"Data" TEXT
);

CREATE INDEX IF NOT EXISTS "IdxTenantContractsInitiatorTenantId" ON "TenantContracts" ("InitiatorTenantId");

CREATE TABLE IF NOT EXISTS "TenantContractParties" (
	"ContractId" TEXT NOT NULL,
	"TenantId" TEXT NOT NULL,
	PRIMARY KEY ("ContractId", "TenantId")
);

CREATE INDEX IF NOT EXISTS "IdxTenantContractPartiesTenantId" ON "TenantContractParties" ("TenantId");
