CREATE TABLE IF NOT EXISTS "${TableScheme}"."Tickets"
(
    "Id"             UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "Title"          TEXT NOT NULL,
    "Description"    TEXT NOT NULL DEFAULT '',
    "TicketType"     INTEGER NOT NULL DEFAULT 0,
    "Status"         INTEGER NOT NULL DEFAULT 0,
    "Priority"       INTEGER NOT NULL DEFAULT 1,
    "AssigneeId"     UUID,
    "ReporterId"     UUID NOT NULL,
    "ConversationId" UUID,
    "MessageId"      UUID,
    "Environment"    INTEGER NOT NULL DEFAULT 2,
    "Tags"           JSONB,
    "CreatedAt"      TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    "UpdatedAt"      TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    "ResolvedAt"     TIMESTAMP WITH TIME ZONE,
    "IsActive"       BOOLEAN NOT NULL DEFAULT TRUE
);
CREATE INDEX IF NOT EXISTS "IX_Tickets_Status" ON "${TableScheme}"."Tickets" ("Status");
CREATE INDEX IF NOT EXISTS "IX_Tickets_AssigneeId" ON "${TableScheme}"."Tickets" ("AssigneeId");
CREATE INDEX IF NOT EXISTS "IX_Tickets_Priority" ON "${TableScheme}"."Tickets" ("Priority");
CREATE INDEX IF NOT EXISTS "IX_Tickets_CreatedAt" ON "${TableScheme}"."Tickets" ("CreatedAt");
