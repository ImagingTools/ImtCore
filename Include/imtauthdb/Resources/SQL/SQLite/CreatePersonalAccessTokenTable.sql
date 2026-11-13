CREATE TABLE IF NOT EXISTS "PersonalAccessTokens"
(
    "Id"         TEXT PRIMARY KEY,
    "Name"       TEXT NOT NULL,
    "Description" TEXT NOT NULL DEFAULT '',
    "UserId"     TEXT,
    "ProductId"  TEXT,
    "ClientProductId" TEXT,
    "TokenHash"  BLOB NOT NULL,
    -- Code stores scopes as comma-separated string, kept as text for compatibility
    "Scopes"     TEXT NOT NULL DEFAULT '',
    "CreatedAt"  TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "LastUsedAt" TEXT,
    "ExpiresAt"  TEXT,
    "Revoked"    INTEGER NOT NULL DEFAULT 0
);

CREATE UNIQUE INDEX IF NOT EXISTS "IX_PersonalAccessTokens_TokenHash"
    ON "PersonalAccessTokens" ("TokenHash");

CREATE INDEX IF NOT EXISTS "IX_PersonalAccessTokens_ClientProductId"
    ON "PersonalAccessTokens" ("ClientProductId") WHERE "ClientProductId" IS NOT NULL;

CREATE INDEX IF NOT EXISTS "IX_PersonalAccessTokens_UserId"
    ON "PersonalAccessTokens" ("UserId");