CREATE TABLE IF NOT EXISTS "OidcAuthCodes"
(
    "Id"                   TEXT PRIMARY KEY,
    "ClientId"             TEXT NOT NULL,
    "UserId"               TEXT NOT NULL,
    "Scope"                TEXT NOT NULL DEFAULT '',
    "RedirectUri"          TEXT NOT NULL DEFAULT '',
    "ExpiresAt"            TEXT NOT NULL,
    "CodeChallenge"        TEXT NOT NULL DEFAULT '',
    "CodeChallengeMethod"  TEXT NOT NULL DEFAULT 'plain',
    "Used"                 INTEGER NOT NULL DEFAULT 0
);