CREATE TABLE IF NOT EXISTS "Messages"
(
    "Id" TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(4)) || '-' || hex(randomblob(2)) || '-' || hex(randomblob(2)) || '-' || hex(randomblob(2)) || '-' || hex(randomblob(6)))),
    "DocumentId" UUID NOT NULL,
    "TypeId" Text, -- optional !
    "Document" jsonb NOT NULL,
    "Name" Text, -- optional !
    "Description" Text, -- optional !
    "RevisionInfo" jsonb, -- OwnerId, OwnerName, OperationDesc, RevisionNumber, Checksum
    "DataMetaInfo" jsonb,
    "Derivates" jsonb,
    "TimeStamp" TEXT NOT NULL,
    "State" TEXT CHECK("State" IN ('Active', 'InActive', 'Disabled'))
);
