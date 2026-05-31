CREATE TABLE IF NOT EXISTS "PersonalAccessTokens"
(
    "Id"         TEXT PRIMARY KEY,
    "Name"       TEXT NOT NULL,
    "Description" TEXT NOT NULL DEFAULT '',
    "UserId"     TEXT NOT NULL,
    "TokenHash"  BLOB NOT NULL,
    -- Code stores scopes as comma-separated string; keep as text for compatibility
    "Scopes"     TEXT NOT NULL DEFAULT '',
    "CreatedAt"  TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "LastUsedAt" TEXT,
    "ExpiresAt"  TEXT,
    "Revoked"    INTEGER NOT NULL DEFAULT 0
);