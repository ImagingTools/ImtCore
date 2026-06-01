CREATE TABLE IF NOT EXISTS "MessageAttachments"
(
    "Id"           TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "MessageId"    TEXT NOT NULL,
    "AttachmentId" TEXT NOT NULL,
    "CreatedAt"    TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
