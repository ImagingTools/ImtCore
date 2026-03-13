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

CREATE INDEX IF NOT EXISTS "${TableName}DocumentIdIndex"
    ON "${TableName}" ("DocumentId");

CREATE INDEX IF NOT EXISTS "${TableName}StateIndex"
    ON "${TableName}" ("State");

CREATE INDEX IF NOT EXISTS "${TableName}RevisionNumberIndex"
    ON "${TableName}" (json_extract("RevisionInfo", '$.RevisionNumber'));
