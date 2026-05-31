CREATE TABLE IF NOT EXISTS "${TableScheme}"."TicketActions"
(
    "Id"                      UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "TicketId"                UUID NOT NULL,
    "UserId"                  UUID,
    "UserName"                TEXT,
    "ActionTypeId"            TEXT,
    "ActionTypeName"          TEXT,
    "ActionTypeDescription"   TEXT,
    "TargetId"                TEXT,
    "TargetTypeId"            TEXT,
    "TargetTypeName"          TEXT,
    "TargetSource"            TEXT,
    "TargetName"              TEXT,
    "Timestamp"               TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    "ActionData"              TEXT
);
CREATE INDEX IF NOT EXISTS "IX_TicketActions_TicketId" ON "${TableScheme}"."TicketActions" ("TicketId");
