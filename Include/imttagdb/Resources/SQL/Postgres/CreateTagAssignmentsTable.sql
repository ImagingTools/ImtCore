CREATE TABLE IF NOT EXISTS "${TableScheme}"."TagAssignments"
(
    "Id"              TEXT PRIMARY KEY,
    "TagId"           TEXT NOT NULL,
    "EntityType"      TEXT NOT NULL,
    "EntityId"        TEXT NOT NULL,
    "CreatedAt"       TIMESTAMP WITHOUT TIME ZONE NOT NULL,
    "CreatedByUserId" TEXT,
    CONSTRAINT "UQ_TagAssignments_Tag_Type_Entity" UNIQUE ("TagId", "EntityType", "EntityId")
);
CREATE INDEX IF NOT EXISTS "IX_TagAssignments_EntityType_EntityId" ON "${TableScheme}"."TagAssignments" ("EntityType", "EntityId");
CREATE INDEX IF NOT EXISTS "IX_TagAssignments_TagId" ON "${TableScheme}"."TagAssignments" ("TagId");
CREATE INDEX IF NOT EXISTS "IX_TagAssignments_EntityType_TagId" ON "${TableScheme}"."TagAssignments" ("EntityType", "TagId");
