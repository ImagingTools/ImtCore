CREATE TABLE IF NOT EXISTS "${TableScheme}"."Conversations"
(
    "Id"               UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "Name"             TEXT NOT NULL,
    "ConversationType" INTEGER NOT NULL DEFAULT 0,
    "Metadata"         JSONB,
    "CreatedAt"        TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    "UpdatedAt"        TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    "IsActive"         BOOLEAN NOT NULL DEFAULT TRUE
);
CREATE INDEX IF NOT EXISTS "IX_Conversations_UpdatedAt" ON "${TableScheme}"."Conversations" ("UpdatedAt");
CREATE INDEX IF NOT EXISTS "IX_Conversations_ConversationType" ON "${TableScheme}"."Conversations" ("ConversationType");
