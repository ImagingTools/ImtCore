CREATE TABLE IF NOT EXISTS "PersonalAccessTokens"
(
    "Id"         uuid PRIMARY KEY,
    "Name"       text        NOT NULL,
    "Description" text       NOT NULL DEFAULT '',
    "UserId"     uuid        NOT NULL,
    "TokenHash"  bytea       NOT NULL,
    -- Code stores scopes as comma-separated string; keep as text for compatibility
    "Scopes"     text        NOT NULL DEFAULT '',
    "CreatedAt"  timestamp without time zone NOT NULL DEFAULT now(),
    "LastUsedAt" timestamp without time zone,
    "ExpiresAt"  timestamp without time zone,
    "Revoked"    boolean     NOT NULL DEFAULT false
);

-- Helpful indexes (optional)
CREATE INDEX IF NOT EXISTS "IX_PersonalAccessTokens_UserId"
    ON "PersonalAccessTokens" ("UserId");

CREATE INDEX IF NOT EXISTS "IX_PersonalAccessTokens_Revoked"
    ON "PersonalAccessTokens" ("Revoked");

CREATE INDEX IF NOT EXISTS "IX_PersonalAccessTokens_ExpiresAt"
    ON "PersonalAccessTokens" ("ExpiresAt");