CREATE TABLE IF NOT EXISTS "Tickets"
(
    "Id"             TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "Title"          TEXT NOT NULL,
    "Description"    TEXT NOT NULL DEFAULT '',
    "TicketType"     INTEGER NOT NULL DEFAULT 0,
    "Status"         INTEGER NOT NULL DEFAULT 0,
    "Priority"       INTEGER NOT NULL DEFAULT 1,
    "AssigneeId"     TEXT,
    "ReporterId"     TEXT NOT NULL,
    "ConversationId" TEXT,
    "MessageId"      TEXT,
    "Environment"    INTEGER NOT NULL DEFAULT 2,
    "Tags"           TEXT,
    "CreatedAt"      TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "UpdatedAt"      TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "ResolvedAt"     TEXT,
    "IsActive"       INTEGER NOT NULL DEFAULT 1
);
CREATE INDEX IF NOT EXISTS "IX_Tickets_Status" ON "Tickets" ("Status");
CREATE INDEX IF NOT EXISTS "IX_Tickets_Priority" ON "Tickets" ("Priority");
