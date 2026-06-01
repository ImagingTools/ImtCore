CREATE TABLE IF NOT EXISTS "MessageStatus"
(
    "Id"         TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "MessageId"  TEXT NOT NULL,
    "UserId"     TEXT NOT NULL,
    "Status"     INTEGER NOT NULL DEFAULT 0,
    "UpdatedAt"  TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);