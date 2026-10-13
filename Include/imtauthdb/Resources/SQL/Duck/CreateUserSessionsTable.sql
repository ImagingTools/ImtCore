CREATE TABLE IF NOT EXISTS "UserSessions"(
    "Id" VARCHAR NOT NULL,
    "RefreshToken" VARCHAR NOT NULL,
    "UserId" VARCHAR (1000) NOT NULL,
    "TenantId" VARCHAR (1000),
    "CreationDate" TIMESTAMP NOT NULL,
    "ExpirationDate" TIMESTAMP NOT NULL,
    PRIMARY KEY ("Id")
);
