CREATE TABLE IF NOT EXISTS "${TableScheme}"."TicketEntityReferences"
(
    "Id"                UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    "TicketId"          UUID NOT NULL,
    "EntityReferenceId" UUID NOT NULL,
    "CreatedAt"         TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT (NOW() AT TIME ZONE 'UTC')
);
CREATE UNIQUE INDEX IF NOT EXISTS "UX_TicketEntityReferences" ON "${TableScheme}"."TicketEntityReferences" ("TicketId", "EntityReferenceId");
