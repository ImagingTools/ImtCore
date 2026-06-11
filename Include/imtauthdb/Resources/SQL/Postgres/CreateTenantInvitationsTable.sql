CREATE TABLE IF NOT EXISTS "TenantInvitations" (
"Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(),
"UserId" UUID NOT NULL,
"TenantId" UUID NOT NULL,
"Role" TEXT NOT NULL,
"Status" INTEGER NOT NULL,
"InvitedByUserId" UUID NOT NULL,
"CreatedAt" TIMESTAMP NOT NULL,
"UpdatedAt" TIMESTAMP NOT NULL,
"ExpiresAt" TIMESTAMP,
"AcceptedAt" TIMESTAMP,
"RejectedAt" TIMESTAMP,
"RevokedAt" TIMESTAMP,
"RevokedByUserId" UUID,
"LastSentAt" TIMESTAMP,
CONSTRAINT "FK_TenantInvitations_TenantId" FOREIGN KEY ("TenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS "IdxTenantInvitationsTenantId" ON "TenantInvitations" ("TenantId");
CREATE INDEX IF NOT EXISTS "IdxTenantInvitationsUserId" ON "TenantInvitations" ("UserId");
CREATE UNIQUE INDEX IF NOT EXISTS "UqTenantInvitationsPending" ON "TenantInvitations" ("TenantId", "UserId") WHERE "Status" = 0;
