CREATE TABLE IF NOT EXISTS "ContractPartyRoles" (
	"Id" TEXT PRIMARY KEY,
	"OwnerTenantId" TEXT NOT NULL,
	"Name" TEXT NOT NULL,
	"IsArchived" INTEGER NOT NULL DEFAULT 0,
	"Data" TEXT
);

CREATE INDEX IF NOT EXISTS "IdxContractPartyRolesOwnerTenantId" ON "ContractPartyRoles" ("OwnerTenantId");
