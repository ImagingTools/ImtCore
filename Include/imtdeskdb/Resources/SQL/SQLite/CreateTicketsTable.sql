CREATE TABLE IF NOT EXISTS "Tickets"
(
    "Id"             TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "Title"          TEXT NOT NULL,
    "Description"    TEXT NOT NULL DEFAULT '',
    "TicketType"     INTEGER NOT NULL DEFAULT 0,
    "Status"         INTEGER NOT NULL DEFAULT 0,
    "StateReason"    INTEGER NOT NULL DEFAULT 0,
    "Priority"       INTEGER NOT NULL DEFAULT 1,
    "AssigneeIds"    TEXT NOT NULL DEFAULT '',
    "ReporterId"     TEXT NOT NULL,
    "ConversationId" TEXT,
    "MessageId"      TEXT,
    "Locked"         INTEGER NOT NULL DEFAULT 0,
    "LockReason"     TEXT,
    "Number"         INTEGER UNIQUE,
    "CreatedAt"      TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "UpdatedAt"      TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "ClosedAt"       TEXT,
    "ResolvedAt"     TEXT
);