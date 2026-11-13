-- ETL watermark per source table, replacing the MDBX RevisionTime map.
CREATE TABLE IF NOT EXISTS "CacheRevision" (
    TableName    VARCHAR PRIMARY KEY,
    LastRevision TIMESTAMP NOT NULL
);
