DO $$
BEGIN
    IF EXISTS (
        SELECT 1 FROM information_schema.tables
        WHERE table_schema = '${TableScheme}'
          AND table_name = 'TenantEntityBindings'
    ) THEN
        RETURN;
    END IF;

    CREATE TABLE "${TableScheme}"."TenantEntityBindings"
    (
        "Id"              TEXT PRIMARY KEY,
        "TenantId"        TEXT NOT NULL,
        "EntityType"      TEXT NOT NULL,
        "EntityId"        TEXT NOT NULL,
        "CreatedAt"       TIMESTAMP WITHOUT TIME ZONE NOT NULL,
        "CreatedByUserId" TEXT,
        CONSTRAINT "UQ_TenantEntityBindings_Tenant_Type_Entity" UNIQUE ("TenantId", "EntityType", "EntityId")
    );

    CREATE INDEX "IX_TenantEntityBindings_TenantId" ON "${TableScheme}"."TenantEntityBindings" ("TenantId");
    CREATE INDEX "IX_TenantEntityBindings_EntityType_EntityId" ON "${TableScheme}"."TenantEntityBindings" ("EntityType", "EntityId");
    CREATE INDEX "IX_TenantEntityBindings_EntityId" ON "${TableScheme}"."TenantEntityBindings" ("EntityId");
END
$$;