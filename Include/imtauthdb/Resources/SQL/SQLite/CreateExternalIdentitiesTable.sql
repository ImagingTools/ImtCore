CREATE TABLE IF NOT EXISTS "ExternalIdentities"
(
    "Id"           TEXT PRIMARY KEY,
    "UserId"       TEXT NOT NULL,
    "ProviderId"   TEXT NOT NULL,
    "Subject"      TEXT NOT NULL,
    "Email"        TEXT NOT NULL DEFAULT '',
    "LinkedAt"     TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "LastSignInAt" TEXT
);

CREATE UNIQUE INDEX IF NOT EXISTS "IX_ExternalIdentities_Provider_Subject"
    ON "ExternalIdentities" ("ProviderId", "Subject");

CREATE INDEX IF NOT EXISTS "IX_ExternalIdentities_UserId"
    ON "ExternalIdentities" ("UserId");
