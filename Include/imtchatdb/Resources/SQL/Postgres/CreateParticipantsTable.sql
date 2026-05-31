CREATE TABLE IF NOT EXISTS "${TableScheme}"."ConversationParticipants"
(
    "Id"             UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "ConversationId" UUID NOT NULL,
    "UserId"         UUID NOT NULL,
    "JoinedAt"       TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW()
);
CREATE UNIQUE INDEX IF NOT EXISTS "UIX_Participants_ConvUser" ON "${TableScheme}"."ConversationParticipants" ("ConversationId", "UserId");
CREATE INDEX IF NOT EXISTS "IX_Participants_UserId" ON "${TableScheme}"."ConversationParticipants" ("UserId");
