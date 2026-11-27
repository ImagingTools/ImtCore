CREATE TABLE IF NOT EXISTS "AssistantConfigurations" (
	"Id" TEXT PRIMARY KEY,
	"TenantId" TEXT NOT NULL,
	"IsEnabled" INTEGER NOT NULL DEFAULT 0,
	"UpdatedAt" TEXT,
	"Data" TEXT
);

CREATE UNIQUE INDEX IF NOT EXISTS "IdxAssistantConfigurationsTenantId" ON "AssistantConfigurations" ("TenantId");
