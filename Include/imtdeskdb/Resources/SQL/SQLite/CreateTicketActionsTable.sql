CREATE TABLE IF NOT EXISTS "TicketActions"
(
    "Id"                      TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "TicketId"                TEXT NOT NULL,
    "UserId"                  TEXT,
    "UserName"                TEXT,
    "ActionTypeId"            TEXT,
    "ActionTypeName"          TEXT,
    "ActionTypeDescription"   TEXT,
    "TargetId"                TEXT,
    "TargetTypeId"            TEXT,
    "TargetTypeName"          TEXT,
    "TargetSource"            TEXT,
    "TargetName"              TEXT,
    "Timestamp"               TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "ActionData"              TEXT
);
CREATE INDEX IF NOT EXISTS "IX_TicketActions_TicketId" ON "TicketActions" ("TicketId");
