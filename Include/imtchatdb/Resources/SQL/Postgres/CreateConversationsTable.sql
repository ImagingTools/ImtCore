CREATE TABLE IF NOT EXISTS "${TableScheme}"."Conversations"
(
    "Id"               UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "Name"             TEXT NOT NULL,
    "ConversationType" INTEGER NOT NULL DEFAULT 0,
    "CreatedAt"        TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC'),
    "UpdatedAt"        TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC')
);
CREATE INDEX IF NOT EXISTS "IX_Conversations_UpdatedAt" ON "${TableScheme}"."Conversations" ("UpdatedAt");
CREATE INDEX IF NOT EXISTS "IX_Conversations_ConversationType" ON "${TableScheme}"."Conversations" ("ConversationType");
