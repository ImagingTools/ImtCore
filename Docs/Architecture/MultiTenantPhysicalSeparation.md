# Physische Datentrennung im Multi-Tenant-System (Option C — Hybrid)

Status: **Umgesetzt** (Phase 1 bis 5; Lasttests und Connection-Pool-Anbindung als Folgearbeit)
Bezug: EU Cyber Resilience Act (Verordnung (EU) 2024/2847), DSGVO Art. 17/20/32 — siehe [CRA_COMPLIANCE.md](../../CRA_COMPLIANCE.md)

## Zielbild

Das Multi-Tenant-System wird von rein logischer Trennung (gemeinsame Tabellen mit
`TenantId`-Spalten und Query-Filterung über `imtauth::ITenantFilterParam`) auf ein
hybrides Modell mit physischer Trennung umgestellt:

- **Shared-Katalog**: Tenant-übergreifende Tabellen verbleiben in einer zentralen
  Katalog-Datenbank bzw. dem Shared-Schema:
  `Tenants`, `TenantMemberships`, `TenantInvitations`, `TenantRelationships`,
  `TenantConnection*`, `TenantPermissions`, `TenantEntityBindings`,
  `CrossTenantMessages`, `Contracts`, `CrossOrgGrants`, `OrderRequests`,
  `UserSessions` sowie die neue Registry-Tabelle `TenantStorage`.
- **Tenant-Daten**: Alle tenant-eigenen Nutzdaten (Dokumente, Collections, Rollen,
  Gruppen, Einstellungen, fachliche Objekte) werden physisch getrennt gespeichert:
  - Standard: eigenes Postgres-Schema pro Tenant (`tenant_<id>`), bei SQLite eine
    eigene Datenbankdatei pro Tenant.
  - Enterprise/Datenresidenz: eigene Datenbank pro Tenant (über dieselbe Abstraktion).
- **Defense in Depth**: Postgres Row-Level Security auf den verbleibenden
  Shared-Tabellen mit `TenantId`-Spalte.

## Storage-Registry (Phase 1 — umgesetzt)

Kernstück ist die Auflösung "Tenant → physischer Speicherort":

| Baustein | Ort | Zweck |
|---|---|---|
| `imtdb::ITenantStorageResolver` | `Include/imtdb/ITenantStorageResolver.h` | Interface: `ResolveTenantStorage`, `RegisterTenantStorage`, `UnregisterTenantStorage`; Datentypen `TenantStorageKind` (SharedSchema/OwnSchema/OwnDatabase/OwnFile), `TenantStorageStatus`, `TenantStorageInfo` |
| `imtdb::CTenantStorageRegistry` | `Include/imtdb/CTenantStorageRegistry.{h,cpp}` | Thread-sichere Registry mit Fail-Closed-Auflösung und Schema-Namensableitung (Sanitisierung auf `[a-z0-9_]`) |
| `imtdb::CTenantStorageResolverComp` | `Include/imtdb/CTenantStorageResolverComp.{h,cpp}` | Komponente (`ImtDatabasePck::TenantStorageResolver`) mit Attributen `AllowSharedFallback` (Default: aus), `SharedSchemaName`, `SchemaNamePrefix`; Audit-Logging aller Registrierungen und fehlgeschlagenen Auflösungen |
| Registry-Tabelle `TenantStorage` | `Include/imtauthdb/Resources/SQL/{Postgres,SQLite}/CreateTenantStorageTable.sql` | Persistente Ablage der Storage-Zuordnungen im Shared-Katalog (`TenantId`, `StorageKind`, `Status`, `SchemaName`, `ConnectionRef`) |
| Tests | `Tests/TenantStorageResolverTest/` | Fail-Closed-Verhalten, Registrierung/Offboarding, Schema-Namens-Sanitisierung, Nebenläufigkeit, Registry-Persistenz (In-Memory-SQLite) |

## Provisionierung & Lifecycle (Phase 2 — umgesetzt)

| Baustein | Ort | Zweck |
|---|---|---|
| `imtdb::ITenantStorageProvisioner` | `Include/imtdb/ITenantStorageProvisioner.h` | Interface: `ProvisionTenantStorage`, `DeprovisionTenantStorage`, `LoadTenantStorageAssignments` |
| `imtdb::CTenantStorageDbStore` | `Include/imtdb/CTenantStorageDbStore.{h,cpp}` | Persistenz der Storage-Zuordnungen in der `TenantStorage`-Tabelle (parametrisierte Queries, UPSERT via `ON CONFLICT`) |
| `imtdb::CTenantStorageProvisionerComp` | `Include/imtdb/CTenantStorageProvisionerComp.{h,cpp}` | Komponente (`ImtDatabasePck::TenantStorageProvisioner`): erstellt bei Postgres ein dediziertes Schema (`CREATE SCHEMA IF NOT EXISTS`), führt die konfigurierten `${TableScheme}`-DDL-Skripte darin aus (`DdlScriptPaths`), persistiert die Zuordnung und registriert sie im Resolver; `DropStorageOnDeprovision` steuert das Löschen des Schemas beim Offboarding (Default: aus) |
| Anbindung Tenant-Lifecycle | `Include/imtauth/CTenantManagerComp.{h,cpp}` | Optionale Referenz `StorageProvisioner`: `CreateTenant` provisioniert den Storage (bei Fehler wird die Tenant-Anlage zurückgerollt), `RemoveTenant` deprovisioniert |

Hinweise:

- Provisionierung ist **idempotent** — ein bereits registrierter Tenant meldet Erfolg.
- Für Nicht-Postgres-Backends (z. B. SQLite) registriert der Provisioner eine explizite
  Shared-Schema-Zuordnung; dedizierte Datenbankdateien pro Tenant folgen in einer
  späteren Phase. Die Fail-Closed-Auflösung bleibt dadurch intakt.
- `LoadTenantStorageAssignments()` lädt beim Start alle persistierten Zuordnungen aus
  der `TenantStorage`-Tabelle in den Resolver.
- Schema-Namen werden ausschließlich aus sanitisierten Tenant-IDs abgeleitet
  (`CTenantStorageRegistry::CreateSchemaName`), bevor sie in DDL-Anweisungen verwendet
  werden (SQL-Injection-Schutz; DDL unterstützt keine Parameter-Bindung).

### Fail-Closed-Semantik

- Auflösung ohne registrierte Zuordnung schlägt fehl (kein stilles Ausweichen auf
  gemeinsame Daten). Nur wenn `AllowSharedFallback` explizit aktiviert ist
  (Übergangsbetrieb/Migration), wird auf das Shared-Schema aufgelöst — die Komponente
  protokolliert dies beim Start als Warnung.
- Leere Tenant-IDs werden immer abgewiesen.
- Dedizierte Zuordnungen (`OwnSchema`/`OwnDatabase`/`OwnFile`) ohne Schema-Name und
  ohne Connection-Referenz werden abgelehnt.

### Audit-Logging (CRA Anhang I Teil I 2(l))

`CTenantStorageResolverComp` protokolliert sicherheitsrelevante Ereignisse über die
Logger-Infrastruktur (`ilog`):

- Registrierung/Aktualisierung einer Storage-Zuordnung (Info, mit Kind/Schema/Connection),
- Entfernung einer Zuordnung (Tenant-Offboarding, Info),
- fehlgeschlagene Auflösungen (Warnung — potenzieller Isolations-Verstoß oder
  Fehlkonfiguration),
- aktivierter Shared-Fallback (Warnung beim Komponentenstart).

## Request-Pfad absichern (Phase 3 — umgesetzt)

| Baustein | Ort | Zweck |
|---|---|---|
| `imtbase::CTenantContextScope` | `Include/imtbase/CTenantContextScope.{h,cpp}` | Thread-lokaler RAII-Scope für den Tenant-Kontext des aktuellen Requests; verschachtelbar, stellt beim Verlassen den vorherigen Kontext wieder her |
| Scope-Aktivierung im Request-Pfad | `Include/imtservergql/CGqlRequestHandlerCompBase.cpp` | `CreateResponse()` aktiviert den Scope zentral für **alle** GraphQL-Handler mit der `TenantId` aus dem `IGqlContext` |
| Tenant-bewusstes `GetTableScheme()` | `Include/imtdb/CSqlDatabaseObjectDelegateCompBase.{h,cpp}` | Optionale Referenz `TenantStorageResolver`: wenn gesetzt, wird das Tabellen-Schema pro Request aus dem Tenant-Kontext über den Resolver aufgelöst; ohne Referenz bleibt das bisherige statische `TableSchema`-Attribut unverändert wirksam |

### Fail-Closed im Request-Pfad

Ist die `TenantStorageResolver`-Referenz an einem SQL-Delegate konfiguriert, liefert
`GetTableScheme()` in folgenden Fällen den Sentinel-Schemanamen
`imt_tenant_storage_denied` (Queries schlagen dadurch fehl, statt versehentlich auf
gemeinsame Daten zuzugreifen), jeweils mit Audit-Fehlermeldung:

- kein aktiver Tenant-Kontext (fehlende `TenantId` im Request),
- Tenant-Storage nicht auflösbar (unbekannter Tenant),
- Storage-Status weder `Active` noch `Migrating` (z. B. `Provisioning`, `Archived`).

Hinweise:

- Delegates mit `TenantStorageResolver`-Referenz sollten `AutoCreateTable` deaktiviert
  lassen — die DDL-Ausführung pro Tenant-Schema übernimmt der Provisioner (Phase 2).
- Cross-Tenant-Funktionen (Shared-Katalog: Tenants, Benutzer, Lizenzen) verwenden
  weiterhin Delegates **ohne** Resolver-Referenz und bleiben unverändert.
- Row-Level Security auf verbleibenden Shared-Tabellen bleibt als zweite
  Verteidigungslinie offen (siehe Threat Model) und kann DB-seitig ergänzt werden.

## Datenmigration (Phase 4 — umgesetzt)

| Baustein | Ort | Zweck |
|---|---|---|
| `imtdb::ITenantDataMigrator` | `Include/imtdb/ITenantDataMigrator.h` | Interface: `MigrateTenantData`, `MigrateAllTenants` |
| `imtdb::CTenantDataMigrator` | `Include/imtdb/CTenantDataMigrator.{h,cpp}` | Kernlogik: kopiert Tenant-Zeilen (`INSERT INTO <tenantSchema>.T SELECT * FROM <sourceSchema>.T WHERE TenantId = :tenantId`), verifiziert Zeilenzahlen, idempotent; Identifier werden validiert und gequotet, Werte ausschließlich parameterisiert |
| `imtdb::CTenantDataMigratorComp` | `Include/imtdb/CTenantDataMigratorComp.{h,cpp}` | Komponente (`ImtDatabasePck::TenantDataMigrator`): löst das Ziel-Schema über den Resolver auf, setzt den Storage-Status auf `Migrating` (persistiert), migriert alle konfigurierten Tabellen (`TableNames`), setzt bei Erfolg `Active`; optional `PurgeSourceRowsAfterMigration` |

Ablauf pro Tenant:

1. Auflösung über den Resolver — nur Tenants mit dediziertem Schema (`OwnSchema`)
   und Status `Active`/`Migrating` werden migriert.
2. Status wird auf `Migrating` gesetzt und persistiert — Phase-3-Guards erlauben
   in diesem Zustand weiterhin Zugriffe (Dual-Read-Übergang).
3. Pro Tabelle: Quell-Zeilen zählen → kopieren → Ziel-Zeilen zählen → verifizieren.
   Bereits vollständig migrierte Tabellen werden übersprungen (idempotenter Re-Run);
   Teilkopien führen zu einem Fehler mit Abbruch (kein stilles Überschreiben).
4. Bei Fehler wird der vorherige Status wiederhergestellt; bei Erfolg wird der Status
   `Active` gesetzt und optional werden die Quell-Zeilen entfernt.

Offen: Migration dateibasierter Dokumente nach `<root>/tenants/<tenantId>/...`,
Checksummen-Verifikation zusätzlich zu Zeilenzahlen.

## Härtung (Phase 5 — umgesetzt)

| Baustein | Ort | Zweck |
|---|---|---|
| `imtdb::CTenantRlsPolicyBuilder` | `Include/imtdb/CTenantRlsPolicyBuilder.{h,cpp}` | Statische Erzeugung validierter RLS-Statements (`ENABLE`/`FORCE ROW LEVEL SECURITY`, `DROP`/`CREATE POLICY`) und der `set_config`-Queries für die Session-Variable; Identifier werden validiert und gequotet, der Tenant-Wert immer als Parameter gebunden |
| `imtdb::ITenantRlsController` | `Include/imtdb/ITenantRlsController.h` | Interface: `ApplyRowLevelSecurity`, `BindSessionTenant`, `UnbindSessionTenant` |
| `imtdb::CTenantRlsControllerComp` | `Include/imtdb/CTenantRlsControllerComp.{h,cpp}` | Komponente (`ImtDatabasePck::TenantRlsController`): wendet die Isolation-Policies auf die konfigurierten Shared-Tabellen an (`TableNames`, `TenantIdColumn`, `SessionVariableName`, Default `app.tenant_id`; optional `ApplyOnStartup`); bindet/löst die Tenant-Session-Variable pro DB-Session; Audit-Logging; Postgres-only (Warnung auf anderen Backends) |
| Verschlüsselte Tablespaces | `Include/imtdb/CTenantStorageProvisionerComp.{h,cpp}` | Neues Attribut `DefaultTablespace`: vor der DDL-Ausführung wird `SET default_tablespace` gesetzt (und danach per `RESET` zurückgesetzt), sodass Tenant-Tabellen auf einem verschlüsselten Volume/Tablespace landen |

### Funktionsweise der RLS-Policies

- Policy pro Tabelle: `USING ("TenantId"::text = current_setting('app.tenant_id', true))`.
- `current_setting(..., true)` liefert `NULL`, wenn die Variable nicht gesetzt ist —
  Sessions ohne gebundenen Tenant sehen **keine** Zeilen (fail-closed).
- `FORCE ROW LEVEL SECURITY` erzwingt die Policy auch für den Tabellen-Eigentümer.
- `BindSessionTenant` setzt die Variable parameterisiert über `set_config` —
  keine String-Konkatenation von Tenant-Werten.
- Wiederholte Anwendung ist idempotent (`DROP POLICY IF EXISTS` vor `CREATE POLICY`).

### Verschlüsselung pro Tenant

- **Postgres**: `DefaultTablespace` am Provisioner auf einen Tablespace legen, der auf
  einem verschlüsselten Volume (LUKS/dm-crypt, EBS-Encryption o. ä.) liegt —
  transparent für den Anwendungscode.
- **SQLite** (dedizierte Tenant-Dateien): verschlüsselte Container oder
  SQLCipher-kompatible Backends; Ablage unter `<root>/tenants/<tenantId>/`.

### Offene Folgearbeiten

- **Lasttests** mit vielen Schemata (Katalog-Größe, Connection-Pooling,
  Migrations-Durchsatz) vor dem Enterprise-Rollout.
- Anbindung von `BindSessionTenant` an das Connection-Pooling (Variable muss pro
  physischer DB-Session gesetzt werden, z. B. beim Ausleihen einer Connection).

## Umsetzungsphasen

1. **Fundament (umgesetzt)** — Resolver-Interface, Registry, Komponente,
   Registry-Tabelle, Tests, Dokumentation.
2. **Provisionierung & Lifecycle (umgesetzt)** — Tenant-Anlage erzeugt Schema und führt
   die vorhandenen `${TableScheme}`-DDL-Skripte aus; Registry-Persistenz über
   `TenantStorage`-Tabelle; Rollback der Tenant-Anlage bei Provisionierungsfehlern;
   Offen: Migrations-Controller-Iteration über alle Tenant-Schemata, Backup/Restore
   pro Tenant.
3. **Request-Pfad absichern (umgesetzt)** — `GetTableScheme()` dynamisch über den
   thread-lokalen Tenant-Kontext (`imtbase::CTenantContextScope`, gesetzt aus dem
   `IGqlContext` in `CGqlRequestHandlerCompBase`) + Resolver; Fail-Closed-Guard
   (fehlende/unbekannte TenantId → Sentinel-Schema, Query schlägt fehl);
   Offen: Row-Level Security auf verbleibenden Shared-Tabellen, explizite
   Cross-Tenant-Pfade (CrossOrgGrants, DelegatedAccess) über den Shared-Katalog.
4. **Datenmigration (umgesetzt)** — `CTenantDataMigratorComp` kopiert pro Tenant die
   Zeilen aus den Shared-Tabellen in das Tenant-Schema, verifiziert Zeilenzahlen und
   schaltet den Status `Migrating` → `Active`; idempotente Re-Runs; optionales
   Aufräumen der Quell-Zeilen. Offen: dateibasierte Dokumente nach
   `<root>/tenants/<tenantId>/...`, Checksummen-Verifikation.
5. **Härtung (umgesetzt)** — Row-Level Security auf Shared-Tabellen via
   `CTenantRlsControllerComp` (Session-Variable `app.tenant_id`, fail-closed);
   verschlüsselte Tablespaces über `DefaultTablespace` am Provisioner.
   Offen: Lasttests mit vielen Schemata, Connection-Pool-Anbindung von
   `BindSessionTenant`.

## CRA-Mapping (Kurzfassung)

| CRA-Anforderung (Anhang I) | Umsetzung |
|---|---|
| Zugriffskontrolle, Schutz vor unbefugtem Zugriff (2(d)) | Physische Trennung pro Tenant + fail-closed Resolver + RLS |
| Vertraulichkeit/Integrität (2(e), (f)) | Getrennte Storage-Bereiche, begrenzter Blast-Radius, RLS auf Shared-Tabellen, optionale Verschlüsselung via `DefaultTablespace` (Phase 5) |
| Datenminimierung (2(g)) | Tenant-Offboarding via `DROP SCHEMA`/Dateilöschung, `UnregisterTenantStorage` |
| Resilienz (2(h), (i)) | Backup/Restore pro Tenant (Phase 2) |
| Angriffsflächenminimierung (2(j)) | Shared-Katalog als dokumentiertes Restrisiko (siehe Threat Model) |
| Security-Logging (2(l)) | Audit-Logging im Resolver; Erweiterung auf Provisionierung/Migration in Phase 2 |

## Threat Model (Restrisiken)

- **Shared-Katalog** bleibt gemeinsame Angriffsfläche: Zugriff nur über Delegates mit
  `TenantId`-Filter; RLS als zweite Verteidigungslinie (Phase 3); minimale Rechte des
  DB-Benutzers.
- **Fehlkonfiguration `AllowSharedFallback`**: Aktivierung deaktiviert den
  Fail-Closed-Schutz; wird deshalb beim Start als Warnung auditiert und ist nur für
  den Migrations-Übergangsbetrieb vorgesehen.
- **Schema-Namenskollisionen**: `CreateSchemaNameForTenant` sanitisiert IDs; bei
  Kollision nach Sanitisierung muss die Provisionierung (Phase 2) eindeutige Namen
  sicherstellen (z. B. UUID-basierte Tenant-IDs).
- **Migrations-Laufzeit** skaliert mit Tenant-Anzahl; Migrationen müssen idempotent
  und pro Tenant wiederaufsetzbar sein.
