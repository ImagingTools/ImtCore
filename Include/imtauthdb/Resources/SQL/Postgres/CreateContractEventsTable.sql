CREATE TABLE IF NOT EXISTS "TenantContractEvents" (
	"Id" TEXT PRIMARY KEY,
	"ContractId" TEXT NOT NULL,
	"TenantId" TEXT,
	"Type" INTEGER NOT NULL,
	"Time" TIMESTAMP,
	"Data" TEXT
);

CREATE INDEX IF NOT EXISTS "IdxTenantContractEventsContractId" ON "TenantContractEvents" ("ContractId");
