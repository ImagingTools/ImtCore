CREATE TABLE IF NOT EXISTS "Messages"
(
    "Id"               TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "ConversationId"   TEXT NOT NULL,
    "SenderId"         TEXT NOT NULL,
    "Content"          TEXT NOT NULL,
    "Status"           INTEGER NOT NULL DEFAULT 0,
    "ReplyToId"        TEXT,
    "CreatedAt"        TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "UpdatedAt"        TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);