CREATE TABLE "Messages"
(
    "Id" INTEGER PRIMARY KEY AUTOINCREMENT,
    "DocumentId" character varying(1000) NOT NULL,
    "Document" jsonb,
    "RevisionNumber" bigint,
    "LastModified" text,
    "Checksum" bigint,
    "IsActive" boolean
);

CREATE INDEX LastModifedIndex ON Messages (
    LastModified DESC
);