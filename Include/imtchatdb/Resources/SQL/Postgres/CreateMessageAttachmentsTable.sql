CREATE TABLE IF NOT EXISTS "${TableScheme}"."MessageAttachments"
(
    "Id"           UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "MessageId"    UUID NOT NULL,
    "AttachmentId" UUID NOT NULL,
    "CreatedAt"    TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC')
);
CREATE UNIQUE INDEX IF NOT EXISTS "UIX_MessageAttachments_MsgAtt" ON "${TableScheme}"."MessageAttachments" ("MessageId", "AttachmentId");
CREATE INDEX IF NOT EXISTS "IX_MessageAttachments_MessageId" ON "${TableScheme}"."MessageAttachments" ("MessageId");
CREATE INDEX IF NOT EXISTS "IX_MessageAttachments_AttachmentId" ON "${TableScheme}"."MessageAttachments" ("AttachmentId");
