CREATE TABLE IF NOT EXISTS "${TableScheme}"."Attachments"
(
    "Id"        UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "FileName"  TEXT NOT NULL,
    "MimeType"  TEXT NOT NULL,
    "Data"      BYTEA NOT NULL,
    "FileSize"  BIGINT NOT NULL DEFAULT 0,
    "CreatedAt" TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC')
);
