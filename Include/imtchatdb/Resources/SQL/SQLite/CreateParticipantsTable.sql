CREATE TABLE IF NOT EXISTS "ConversationParticipants"
(
    "Id"             TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "ConversationId" TEXT NOT NULL,
    "UserId"         TEXT NOT NULL,
    "JoinedAt"       TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);