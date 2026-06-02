CREATE TABLE IF NOT EXISTS "TicketEntityReferences"
(
    "Id"                TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "TicketId"          TEXT NOT NULL,
    "EntityReferenceId" TEXT NOT NULL,
    "CreatedAt"         TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);