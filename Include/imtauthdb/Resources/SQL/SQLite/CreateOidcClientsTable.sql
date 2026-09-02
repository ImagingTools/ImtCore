CREATE TABLE IF NOT EXISTS "OidcClients"
(
    "Id"               TEXT PRIMARY KEY,
    "ClientSecretHash" BLOB NOT NULL DEFAULT '',
    "ClientName"       TEXT NOT NULL DEFAULT '',
    "RedirectUris"     TEXT NOT NULL DEFAULT '',
    "GrantTypes"       TEXT NOT NULL DEFAULT '',
    "Scopes"           TEXT NOT NULL DEFAULT '',
    "CreatedAt"        TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);