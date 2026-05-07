ALTER TABLE ${TableScheme}."${TableName}" ADD COLUMN IF NOT EXISTS "TenantId" Text;

CREATE INDEX IF NOT EXISTS "${TableName}TenantIdIndex"
    ON ${TableScheme}."${TableName}" USING btree
    ("TenantId" ASC NULLS LAST)
    TABLESPACE pg_default;
