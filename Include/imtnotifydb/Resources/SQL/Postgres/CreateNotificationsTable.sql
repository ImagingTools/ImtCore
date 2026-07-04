CREATE TABLE IF NOT EXISTS "${TableScheme}"."Notifications"
(
    "Id"            UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "RecipientId"   UUID NOT NULL,
    "Category"      TEXT,
    "Title"         TEXT NOT NULL,
    "Body"          TEXT,
    "IconName"      TEXT,
    "Severity"      INTEGER NOT NULL DEFAULT 0,
    "IsRead"        INTEGER NOT NULL DEFAULT 0,
    "SourceType"    TEXT,
    "SourceId"      TEXT,
    "TargetRoute"   TEXT,
    "Metadata"      TEXT,
    "CreatedAt"     TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC'),
    "UpdatedAt"     TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC')
);
