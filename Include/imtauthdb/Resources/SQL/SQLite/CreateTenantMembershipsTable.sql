CREATE TABLE IF NOT EXISTS "TenantMemberships"
(
    "Id"          TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "UserId"      TEXT NOT NULL,
    "TenantId"    TEXT NOT NULL,
    "Role"        TEXT NOT NULL,
    "IsActive"    INTEGER NOT NULL DEFAULT 1,
    "JoinedAt"    TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "UpdatedAt"   TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY ("TenantId") REFERENCES "Tenants" ("Id") ON DELETE CASCADE
);