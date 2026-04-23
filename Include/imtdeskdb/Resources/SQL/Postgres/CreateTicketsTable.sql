CREATE TABLE IF NOT EXISTS "${TableScheme}"."Tickets"
(
    "Id"             UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "Title"          TEXT NOT NULL,
    "Description"    TEXT NOT NULL DEFAULT '',
    "TicketType"     INTEGER NOT NULL DEFAULT 0,
    "Status"         INTEGER NOT NULL DEFAULT 0,
    "StateReason"    INTEGER NOT NULL DEFAULT 0,
    "Priority"       INTEGER NOT NULL DEFAULT 1,
    "AssigneeIds"    TEXT NOT NULL DEFAULT '',
    "ReporterId"     UUID NOT NULL,
    "ConversationId" UUID,
    "MessageId"      UUID,
    "Locked"         BOOLEAN NOT NULL DEFAULT FALSE,
    "LockReason"     TEXT,
    "Number"         SERIAL NOT NULL UNIQUE,
    "CreatedAt"      TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC'),
    "UpdatedAt"      TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC'),
    "ClosedAt"       TIMESTAMP WITHOUT TIME ZONE,
    "ResolvedAt"     TIMESTAMP WITHOUT TIME ZONE
);
CREATE INDEX IF NOT EXISTS "IX_Tickets_Status" ON "${TableScheme}"."Tickets" ("Status");
CREATE INDEX IF NOT EXISTS "IX_Tickets_Priority" ON "${TableScheme}"."Tickets" ("Priority");
CREATE INDEX IF NOT EXISTS "IX_Tickets_CreatedAt" ON "${TableScheme}"."Tickets" ("CreatedAt");
CREATE INDEX IF NOT EXISTS "IX_Tickets_Number" ON "${TableScheme}"."Tickets" ("Number");
