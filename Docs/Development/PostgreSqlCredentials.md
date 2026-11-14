# PostgreSQL credentials and connection security

This page describes how `imtdb::CDatabaseEngineComp` handles PostgreSQL credentials and how to deploy a server securely.

## Behavior in ImtCore

- **No default password.** The `Password` attribute of `CDatabaseAccessSettingsComp`, `CDatabaseEngineComp` and `CMongoDatabaseEngineComp` is empty by default, and so are the exported `Password` / `DefaultDatabasePassword` values in `DatabaseEngine.acc` and `ServerSettings.acc`.
  With an empty password the QPSQL driver does not pass one to `libpq`, so `libpq` uses `peer` authentication over the Unix socket or a password from `PGPASSFILE` / `~/.pgpass`.
- **The password is never logged.** Connection details are not printed before the database is created.
- **The password is write-only in the settings UI.** `CDatabaseAccessSettingsRepresentationControllerComp` does not send the password to the client. An empty password from the client means "unchanged"; a non-empty one replaces the stored password. To remove a stored password, edit the settings file.
- **SQL text is not part of error messages.** A failed query reports the database error; the SQL statement and bind values go only to verbose/debug output.

## TLS to PostgreSQL

- If the `COF_SSL` connection flag is set (`UseSSL` attribute of `CDatabaseAccessSettingsComp`), the QPSQL connection uses `sslmode=require`: the channel is encrypted, but the server certificate is **not** verified.
- To verify the server certificate, set the `ConnectOptions` attribute of `CDatabaseEngineComp`. Options are separated by `;` and are passed to `libpq` as is, after the flag-based options, so they take precedence:

```
sslmode=verify-full;sslrootcert=/etc/<PRODUCT-NAME>/db-ca.crt
```

## Recommended deployment

PostgreSQL on the same host:

- connect over the Unix socket (`HostName` = `/var/run/postgresql`) with `peer` authentication and an empty password;
- use a dedicated role (`NOSUPERUSER NOCREATEDB NOCREATEROLE`) that owns only the application database;
- create the database in advance and set `AutoCreateDatabase` to `0`;
- set `listen_addresses = ''` or `'localhost'` and `password_encryption = scram-sha-256`.

PostgreSQL on another host:

- keep the password out of the settings file: put it into a `0600` pgpass file and point `PGPASSFILE` to it, for example with systemd `LoadCredential=pgpass:/etc/<app>/pgpass` and `Environment=PGPASSFILE=%d/pgpass`;
- do not pass the password in `PGPASSWORD`, on the command line, or in version control;
- use `ConnectOptions` = `sslmode=verify-full;sslrootcert=<CA file>`, `hostssl ... scram-sha-256` in `pg_hba.conf`, and a firewall rule that allows port 5432 only from the application host.

In both cases run the server under a dedicated system user and keep the settings file readable only by that user (`0600`).

## Not covered here

- The location of the settings file (`CSystemLocationComp` in ACF resolves the shared application data folder to `C:/Users/Public` or `/Users/Shared`).
- Application migrations that change table ownership to `postgres` and thus require superuser rights.
