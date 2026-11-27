CREATE TABLE IF NOT EXISTS "TenantContractPartyRecords" (
	"Id" TEXT PRIMARY KEY,
	"ContractId" TEXT NOT NULL,
	"TenantId" TEXT NOT NULL,
	"UpdatedAt" TEXT,
	"Data" TEXT
);

CREATE INDEX IF NOT EXISTS "IdxTenantContractPartyRecordsContractId" ON "TenantContractPartyRecords" ("ContractId");
