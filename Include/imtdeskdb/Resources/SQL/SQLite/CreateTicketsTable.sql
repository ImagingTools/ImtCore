CREATE TABLE IF NOT EXISTS "Tickets"
(
    "Id"             TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "Title"          TEXT NOT NULL,
    "Description"    TEXT NOT NULL DEFAULT '',
    "TicketType"     INTEGER NOT NULL DEFAULT 0,
    "Status"         INTEGER NOT NULL DEFAULT 0,
    "StateReason"    INTEGER NOT NULL DEFAULT 0,
    "Priority"       INTEGER NOT NULL DEFAULT 1,
    "AssigneeIds"    TEXT,
    "ReporterId"     TEXT NOT NULL,
    "ConversationId" TEXT,
    "MessageId"      TEXT,
    "Tags"           TEXT,
    "LabelIds"       TEXT,
    "Locked"         INTEGER NOT NULL DEFAULT 0,
    "LockReason"     TEXT,
    "Number"         INTEGER UNIQUE,
    "CreatedAt"      TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "UpdatedAt"      TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "ClosedAt"       TEXT,
    "ResolvedAt"     TEXT
);
CREATE INDEX IF NOT EXISTS "IX_Tickets_Status" ON "Tickets" ("Status");
CREATE INDEX IF NOT EXISTS "IX_Tickets_Priority" ON "Tickets" ("Priority");
CREATE INDEX IF NOT EXISTS "IX_Tickets_Number" ON "Tickets" ("Number");
CREATE TRIGGER IF NOT EXISTS "TR_Tickets_AutoNumber"
AFTER INSERT ON "Tickets"
FOR EACH ROW
WHEN NEW."Number" IS NULL
BEGIN
    UPDATE "Tickets" SET "Number" = (SELECT COALESCE(MAX("Number"), 0) + 1 FROM "Tickets") WHERE "Id" = NEW."Id";
END;
