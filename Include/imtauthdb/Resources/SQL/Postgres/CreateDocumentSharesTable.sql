DO $$
BEGIN
    IF EXISTS (
        SELECT 1 FROM information_schema.tables
        WHERE table_schema = '${TableScheme}'
          AND table_name = 'DocumentShares'
    ) THEN
        RETURN;
    END IF;

    CREATE TABLE "${TableScheme}"."DocumentShares"
    (
        "Id"              TEXT PRIMARY KEY,
        "CollectionId"    TEXT NOT NULL,
        "EntityId"        TEXT NOT NULL,
        "EntityName"      TEXT,
        "OwnerTenantId"   TEXT NOT NULL,
        "OwnerTenantName" TEXT,
        "TargetTenantId"  TEXT NOT NULL,
        "TargetTenantName" TEXT,
        "TargetGroupIds"  TEXT NOT NULL DEFAULT '',
        "AccessLevel"     INTEGER NOT NULL,
        "Status"          INTEGER NOT NULL,
        "ValidUntil"      TIMESTAMP WITHOUT TIME ZONE,
        "Message"         TEXT,
        "CreatedByUserId" TEXT,
        "CreatedAt"       TIMESTAMP WITHOUT TIME ZONE NOT NULL,
        "UpdatedAt"       TIMESTAMP WITHOUT TIME ZONE,
        CONSTRAINT "UQ_DocumentShares_Entity_Target" UNIQUE ("EntityId", "TargetTenantId")
    );

    CREATE INDEX "IX_DocumentShares_TargetTenantId_Status" ON "${TableScheme}"."DocumentShares" ("TargetTenantId", "Status");
    CREATE INDEX "IX_DocumentShares_OwnerTenantId" ON "${TableScheme}"."DocumentShares" ("OwnerTenantId");
END
$$;
