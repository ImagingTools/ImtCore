CREATE TABLE IF NOT EXISTS "Notifications"
(
    "Id"            TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "RecipientId"   TEXT NOT NULL,
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
    "CreatedAt"     TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "UpdatedAt"     TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
