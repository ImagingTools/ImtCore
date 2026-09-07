CREATE TABLE IF NOT EXISTS "OidcTokens"
(
    "Id"             uuid PRIMARY KEY,
    "ClientId"       uuid        NOT NULL,
    "UserId"         uuid,
    "Scope"          text        NOT NULL DEFAULT '',
    "TokenType"      text        NOT NULL DEFAULT 'access',
    "ExpiresAt"      timestamp without time zone NOT NULL,
    "IsRevoked"      boolean     NOT NULL DEFAULT false,
    "CreatedAt"      timestamp without time zone NOT NULL DEFAULT now()
);

CREATE INDEX IF NOT EXISTS "IX_OidcTokens_ClientId"
    ON "OidcTokens" ("ClientId");

CREATE INDEX IF NOT EXISTS "IX_OidcTokens_UserId"
    ON "OidcTokens" ("UserId");

CREATE INDEX IF NOT EXISTS "IX_OidcTokens_IsRevoked"
    ON "OidcTokens" ("IsRevoked");

CREATE INDEX IF NOT EXISTS "IX_OidcTokens_ExpiresAt"
    ON "OidcTokens" ("ExpiresAt");