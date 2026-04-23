CREATE TABLE IF NOT EXISTS "MessageAttachments"
(
    "Id"           TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "MessageId"    TEXT NOT NULL,
    "AttachmentId" TEXT NOT NULL,
    "CreatedAt"    TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
CREATE UNIQUE INDEX IF NOT EXISTS "UIX_MessageAttachments_MsgAtt" ON "MessageAttachments" ("MessageId", "AttachmentId");
CREATE INDEX IF NOT EXISTS "IX_MessageAttachments_MessageId" ON "MessageAttachments" ("MessageId");
CREATE INDEX IF NOT EXISTS "IX_MessageAttachments_AttachmentId" ON "MessageAttachments" ("AttachmentId");
