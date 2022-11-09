CREATE TABLE IF NOT EXISTS "Tests"(
    Id VARCHAR (1000) NOT NULL,
	Name VARCHAR (1000),
	Description VARCHAR (1000),
	Added timestamp,
	LastModified timestamp,
	PRIMARY KEY (Id)
);