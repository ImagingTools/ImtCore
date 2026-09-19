CREATE TABLE IF NOT EXISTS "OidcTokens"
(
    "Id"             TEXT PRIMARY KEY,
    "ClientId"       TEXT NOT NULL,
    "UserId"         TEXT,
    "Scope"          TEXT NOT NULL DEFAULT '',
    "TokenType"      TEXT NOT NULL DEFAULT 'access',
    "ExpiresAt"      TEXT NOT NULL,
    "IsRevoked"      INTEGER NOT NULL DEFAULT 0,
    "CreatedAt"      TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);