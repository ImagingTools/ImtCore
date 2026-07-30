CREATE TABLE IF NOT EXISTS "DocumentShares" (
	"Id" TEXT PRIMARY KEY,
	"SourceTenantId" TEXT NOT NULL,
	"TargetTenantId" TEXT NOT NULL,
	"RelationshipId" TEXT,
	"DocumentId" TEXT NOT NULL,
	"DocumentType" TEXT,
	"DocumentName" TEXT,
	"AccessMode" INTEGER NOT NULL DEFAULT 0,
	"Status" INTEGER NOT NULL DEFAULT 0,
	"SharedByUserId" TEXT,
	"CreatedAt" TEXT NOT NULL,
	"UpdatedAt" TEXT NOT NULL,
	"ExpiresAt" TEXT,
	"RevokedAt" TEXT,
	FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
	FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
	FOREIGN KEY ("RelationshipId") REFERENCES "TenantRelationships" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxDocumentSharesSourceTenantId" ON "DocumentShares" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxDocumentSharesTargetTenantId" ON "DocumentShares" ("TargetTenantId");
CREATE INDEX IF NOT EXISTS "IdxDocumentSharesRelationshipId" ON "DocumentShares" ("RelationshipId");
CREATE INDEX IF NOT EXISTS "IdxDocumentSharesDocumentId" ON "DocumentShares" ("DocumentId");
