CREATE TABLE IF NOT EXISTS "Conversations"
(
    "Id"               TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "Name"             TEXT NOT NULL,
    "ConversationType" INTEGER NOT NULL DEFAULT 0,
    "Metadata"         TEXT,
    "CreatedAt"        TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "UpdatedAt"        TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX IF NOT EXISTS "IX_Conversations_UpdatedAt" ON "Conversations" ("UpdatedAt");
