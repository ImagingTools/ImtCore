CREATE TABLE IF NOT EXISTS "TenantDirectoryProfiles" (
	"Id" TEXT PRIMARY KEY,
	"TenantId" TEXT NOT NULL,
	"IsListed" INTEGER NOT NULL DEFAULT 0,
	"SearchText" TEXT NOT NULL DEFAULT '',
	"UpdatedAt" TEXT,
	"Data" TEXT
);

CREATE INDEX IF NOT EXISTS "IdxTenantDirectoryProfilesIsListed" ON "TenantDirectoryProfiles" ("IsListed");
