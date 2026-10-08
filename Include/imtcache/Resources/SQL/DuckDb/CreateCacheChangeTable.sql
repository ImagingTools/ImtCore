-- The keys of the rows the mirror tables changed, for the tables derived from them.
-- The sequence orders the entries.
CREATE SEQUENCE IF NOT EXISTS "CacheChangeSeq";

CREATE TABLE IF NOT EXISTS "CacheChange" (
    ChangeId BIGINT NOT NULL,
    TableName VARCHAR NOT NULL,
    -- Surrogate id of the changed row; NULL says the whole table was rebuilt.
    KeyId UBIGINT
);
