CREATE TABLE IF NOT EXISTS "Messages"
(
    "Id"               TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "ConversationId"   TEXT NOT NULL,
    "SenderId"         TEXT NOT NULL,
    "Content"          TEXT NOT NULL,
    "EntityReferences" TEXT,
    "AttachmentIds"    TEXT,
    "Status"           INTEGER NOT NULL DEFAULT 0,
    "CreatedAt"        TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "UpdatedAt"        TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "IsActive"         INTEGER NOT NULL DEFAULT 1
);
CREATE INDEX IF NOT EXISTS "IX_Messages_ConversationId" ON "Messages" ("ConversationId");
CREATE INDEX IF NOT EXISTS "IX_Messages_SenderId" ON "Messages" ("SenderId");
CREATE INDEX IF NOT EXISTS "IX_Messages_CreatedAt" ON "Messages" ("CreatedAt");
