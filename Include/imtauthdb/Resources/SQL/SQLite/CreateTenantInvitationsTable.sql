CREATE TABLE IF NOT EXISTS "TenantInvitations" (
	"Id" TEXT PRIMARY KEY,
	"UserId" TEXT NOT NULL,
	"TenantId" TEXT NOT NULL,
	"Role" TEXT NOT NULL,
	"Status" INTEGER NOT NULL,
	"InvitedByUserId" TEXT NOT NULL,
	"CreatedAt" TEXT NOT NULL,
	"UpdatedAt" TEXT NOT NULL,
	"ExpiresAt" TEXT,
	"AcceptedAt" TEXT,
	"RejectedAt" TEXT,
	"RevokedAt" TEXT,
	"RevokedByUserId" TEXT,
	"LastSentAt" TEXT,
	FOREIGN KEY ("TenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);