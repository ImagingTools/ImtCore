CREATE TABLE IF NOT EXISTS "${TableScheme}"."MessageStatus"
(
    "Id"             UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "MessageId"      UUID NOT NULL,
    "UserId"         UUID NOT NULL,
    "Status"         INTEGER NOT NULL DEFAULT 0,
    "UpdatedAt"      TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW()
);
CREATE UNIQUE INDEX IF NOT EXISTS "UIX_MessageStatus_MessageUser" ON "${TableScheme}"."MessageStatus" ("MessageId", "UserId");
CREATE INDEX IF NOT EXISTS "IX_MessageStatus_UserId" ON "${TableScheme}"."MessageStatus" ("UserId");
