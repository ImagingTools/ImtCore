CREATE TABLE IF NOT EXISTS "Attachments"
(
    "Id"        TEXT PRIMARY KEY DEFAULT (lower(hex(randomblob(16)))),
    "FileName"  TEXT NOT NULL,
    "MimeType"  TEXT NOT NULL,
    "Data"      BLOB NOT NULL,
    "FileSize"  INTEGER NOT NULL DEFAULT 0,
    "CreatedAt" TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
