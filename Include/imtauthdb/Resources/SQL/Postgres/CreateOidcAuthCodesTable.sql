CREATE TABLE IF NOT EXISTS "OidcAuthCodes"
(
    "Id"                   text PRIMARY KEY,
    "ClientId"             uuid        NOT NULL,
    "UserId"               uuid        NOT NULL,
    "Scope"                text        NOT NULL DEFAULT '',
    "RedirectUri"          text        NOT NULL DEFAULT '',
    "ExpiresAt"            timestamp without time zone NOT NULL,
    "CodeChallenge"        text        NOT NULL DEFAULT '',
    "CodeChallengeMethod"  text        NOT NULL DEFAULT 'plain',
    "Used"                 boolean     NOT NULL DEFAULT false
);

CREATE INDEX IF NOT EXISTS "IX_OidcAuthCodes_ClientId"
    ON "OidcAuthCodes" ("ClientId");

CREATE INDEX IF NOT EXISTS "IX_OidcAuthCodes_ExpiresAt"
    ON "OidcAuthCodes" ("ExpiresAt");