CREATE TABLE IF NOT EXISTS "ExternalIdentities"
(
    "Id"              uuid PRIMARY KEY,
    "UserId"          uuid NOT NULL,
    "Provider"        text NOT NULL,
    "ExternalSubject" text NOT NULL,
    "ExternalEmail"   text DEFAULT '',
    "LinkedAt"        timestamp without time zone NOT NULL DEFAULT now(),
    "LastAuthAt"      timestamp without time zone
);

CREATE UNIQUE INDEX IF NOT EXISTS "IX_ExtId_Provider_Subject"
    ON "ExternalIdentities" ("Provider", "ExternalSubject");

CREATE INDEX IF NOT EXISTS "IX_ExtId_UserId"
    ON "ExternalIdentities" ("UserId");
