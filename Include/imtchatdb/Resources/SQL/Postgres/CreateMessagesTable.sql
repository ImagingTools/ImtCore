CREATE TABLE IF NOT EXISTS "${TableScheme}"."Messages"
(
    "Id"               UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "ConversationId"   UUID NOT NULL,
    "SenderId"         UUID NOT NULL,
    "Content"          TEXT NOT NULL,
    "EntityReferences" JSONB,
    "AttachmentIds"    JSONB,
    "Status"           INTEGER NOT NULL DEFAULT 0,
    "CreatedAt"        TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    "UpdatedAt"        TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    "IsActive"         BOOLEAN NOT NULL DEFAULT TRUE
);
CREATE INDEX IF NOT EXISTS "IX_Messages_ConversationId" ON "${TableScheme}"."Messages" ("ConversationId");
CREATE INDEX IF NOT EXISTS "IX_Messages_SenderId" ON "${TableScheme}"."Messages" ("SenderId");
CREATE INDEX IF NOT EXISTS "IX_Messages_CreatedAt" ON "${TableScheme}"."Messages" ("CreatedAt");
CREATE INDEX IF NOT EXISTS "IX_Messages_Content_Fts" ON "${TableScheme}"."Messages" USING gin(to_tsvector('simple', "Content"));
