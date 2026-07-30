CREATE TABLE IF NOT EXISTS "DocumentShares" (
"Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
"SourceTenantId" UUID NOT NULL,
"TargetTenantId" UUID NOT NULL,
"RelationshipId" UUID,
"DocumentId" TEXT NOT NULL,
"DocumentType" TEXT,
"DocumentName" TEXT,
"AccessMode" INTEGER NOT NULL DEFAULT 0,
"Status" INTEGER NOT NULL DEFAULT 0,
"SharedByUserId" TEXT,
"CreatedAt" TIMESTAMP NOT NULL,
"UpdatedAt" TIMESTAMP NOT NULL,
"ExpiresAt" TIMESTAMP,
"RevokedAt" TIMESTAMP,
CONSTRAINT "FK_DocumentShares_SourceTenantId" FOREIGN KEY ("SourceTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_DocumentShares_TargetTenantId" FOREIGN KEY ("TargetTenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE,
CONSTRAINT "FK_DocumentShares_RelationshipId" FOREIGN KEY ("RelationshipId") REFERENCES "TenantRelationships" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxDocumentSharesSourceTenantId" ON "DocumentShares" ("SourceTenantId");
CREATE INDEX IF NOT EXISTS "IdxDocumentSharesTargetTenantId" ON "DocumentShares" ("TargetTenantId");
CREATE INDEX IF NOT EXISTS "IdxDocumentSharesRelationshipId" ON "DocumentShares" ("RelationshipId");
CREATE INDEX IF NOT EXISTS "IdxDocumentSharesDocumentId" ON "DocumentShares" ("DocumentId");
