CREATE TABLE IF NOT EXISTS "UserNotifications"
(
    "Id"            TEXT PRIMARY KEY,
    "ProductId"     TEXT NOT NULL DEFAULT '',
    "TenantId"      TEXT NOT NULL DEFAULT '',
    "RecipientId"   TEXT NOT NULL,
    "TypeId"        TEXT NOT NULL,
    "Severity"      INTEGER NOT NULL DEFAULT 0,
    "Actors"        TEXT NOT NULL DEFAULT '[]',
    "SubjectId"     TEXT NOT NULL DEFAULT '',
    "SubjectTypeId" TEXT NOT NULL DEFAULT '',
    "SubjectName"   TEXT NOT NULL DEFAULT '',
    "Arguments"     TEXT NOT NULL DEFAULT '[]',
    "Body"          TEXT NOT NULL DEFAULT '',
    "SearchText"    TEXT NOT NULL DEFAULT '',
    "GroupKey"      TEXT NOT NULL DEFAULT '',
    "GroupCount"    INTEGER NOT NULL DEFAULT 1,
    "CreatedAt"     TEXT NOT NULL,
    "DeliveredAt"   TEXT NULL,
    "ReadAt"        TEXT NULL
);
CREATE INDEX IF NOT EXISTS "IX_UserNotifications_Inbox" ON "UserNotifications" ("RecipientId", "CreatedAt", "Id");
CREATE INDEX IF NOT EXISTS "IX_UserNotifications_Undelivered" ON "UserNotifications" ("RecipientId", "CreatedAt", "Id") WHERE "DeliveredAt" IS NULL;
CREATE INDEX IF NOT EXISTS "IX_UserNotifications_Unread" ON "UserNotifications" ("ProductId", "TenantId", "RecipientId") WHERE "ReadAt" IS NULL;
CREATE UNIQUE INDEX IF NOT EXISTS "UIX_UserNotifications_OpenGroup" ON "UserNotifications" ("ProductId", "TenantId", "RecipientId", "GroupKey") WHERE "ReadAt" IS NULL AND "GroupKey" <> '';
CREATE INDEX IF NOT EXISTS "IX_UserNotifications_Group" ON "UserNotifications" ("ProductId", "TenantId", "GroupKey") WHERE "GroupKey" <> '';

CREATE TABLE IF NOT EXISTS "UserNotificationDedupKeys"
(
    "ProductId"     TEXT NOT NULL DEFAULT '',
    "TenantId"      TEXT NOT NULL DEFAULT '',
    "RecipientId"   TEXT NOT NULL,
    "DedupKey"      TEXT NOT NULL,
    "CreatedAt"     TEXT NOT NULL,
    PRIMARY KEY ("ProductId", "TenantId", "RecipientId", "DedupKey")
);
CREATE INDEX IF NOT EXISTS "IX_UserNotificationDedupKeys_Created" ON "UserNotificationDedupKeys" ("CreatedAt");
CREATE INDEX IF NOT EXISTS "IX_UserNotificationDedupKeys_Key" ON "UserNotificationDedupKeys" ("ProductId", "TenantId", "DedupKey");
CREATE INDEX IF NOT EXISTS "IX_UserNotificationDedupKeys_Recipient" ON "UserNotificationDedupKeys" ("RecipientId", "TenantId");
