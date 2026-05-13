CREATE TABLE IF NOT EXISTS UserSessions (
    Id TEXT PRIMARY KEY,
    RefreshToken TEXT NOT NULL,
    UserId TEXT NOT NULL,
    TenantId TEXT,
    CreationDate TEXT NOT NULL,
    ExpirationDate TEXT NOT NULL
);
