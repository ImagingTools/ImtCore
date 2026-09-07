CREATE TABLE IF NOT EXISTS "OidcClients"
(
    "Id"               uuid PRIMARY KEY,
    "ClientSecretHash" bytea       NOT NULL DEFAULT '',
    "ClientName"       text        NOT NULL DEFAULT '',
    "RedirectUris"     text        NOT NULL DEFAULT '',
    "GrantTypes"       text        NOT NULL DEFAULT '',
    "Scopes"           text        NOT NULL DEFAULT '',
    "CreatedAt"        timestamp without time zone NOT NULL DEFAULT now()
);

CREATE INDEX IF NOT EXISTS "IX_OidcClients_ClientName"
    ON "OidcClients" ("ClientName");