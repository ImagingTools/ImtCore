CREATE TABLE IF NOT EXISTS "ExternalIdentities"
(
    "Id"              TEXT PRIMARY KEY,
    "UserId"          TEXT NOT NULL,
    "Provider"        TEXT NOT NULL,
    "ExternalSubject" TEXT NOT NULL,
    "ExternalEmail"   TEXT DEFAULT '',
    "LinkedAt"        TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "LastAuthAt"      TEXT
);

CREATE UNIQUE INDEX IF NOT EXISTS "IX_ExtId_Provider_Subject"
    ON "ExternalIdentities" ("Provider", "ExternalSubject");

CREATE INDEX IF NOT EXISTS "IX_ExtId_UserId"
    ON "ExternalIdentities" ("UserId");
