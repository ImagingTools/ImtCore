CREATE TABLE IF NOT EXISTS "${TableName}"
(
    "Id" TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "DocumentId" TEXT NOT NULL,
    "TypeId" TEXT, -- optional !
    "Document" TEXT NOT NULL,
    "Name" TEXT, -- optional !
    "Description" TEXT, -- optional !
    "RevisionInfo" TEXT, -- OwnerId, OwnerName, OperationDesc, RevisionNumber, Checksum
    "DataMetaInfo" TEXT,
    "Derivates" TEXT,
    "TimeStamp" TEXT NOT NULL, -- UTC Time
    "State" TEXT
);

CREATE INDEX IF NOT EXISTS "${TableName}RevisionNumberIndex"
    ON "${TableName}" (CAST(json_extract("RevisionInfo", '$.RevisionNumber') AS INTEGER));

CREATE INDEX IF NOT EXISTS "${TableName}DocumentStateRevisionIndex"
    ON "${TableName}" ("DocumentId", "State", CAST(json_extract("RevisionInfo", '$.RevisionNumber') AS INTEGER) DESC);

CREATE INDEX IF NOT EXISTS "${TableName}InitialRevisionTimeIndex"
    ON "${TableName}" ("DocumentId", "TimeStamp" DESC)
    WHERE CAST(json_extract("RevisionInfo", '$.RevisionNumber') AS INTEGER) = 1;
