CREATE TABLE IF NOT EXISTS "Revisions" (
    Revision bigint NOT NULL PRIMARY KEY,
	CreationDate timestamp DEFAULT current_timestamp,
	Description VARCHAR (1000)
);