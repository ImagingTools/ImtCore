-- The last CacheChange entry each derived table has taken into account.
CREATE TABLE IF NOT EXISTS "CacheChangeCursor" (
    Consumer VARCHAR PRIMARY KEY,
    LastChangeId BIGINT NOT NULL
);
