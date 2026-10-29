CREATE TABLE IF NOT EXISTS "ExternalIdentities"
(
    "Id"           uuid PRIMARY KEY,
    "UserId"       uuid NOT NULL,
    "ProviderId"   text NOT NULL,
    "Subject"      text NOT NULL,
    "Email"        text NOT NULL DEFAULT '',
    "LinkedAt"     timestamp without time zone NOT NULL DEFAULT now(),
    "LastSignInAt" timestamp without time zone
);

CREATE UNIQUE INDEX IF NOT EXISTS "IX_ExternalIdentities_Provider_Subject"
    ON "ExternalIdentities" ("ProviderId", "Subject");

CREATE INDEX IF NOT EXISTS "IX_ExternalIdentities_UserId"
    ON "ExternalIdentities" ("UserId");
