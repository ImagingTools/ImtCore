# Physische Datentrennung im Multi-Tenant-System (Option C — Hybrid)

Status: **In Umsetzung** (Phase 1 und 2)
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

## Umsetzungsphasen

1. **Fundament (umgesetzt)** — Resolver-Interface, Registry, Komponente,
   Registry-Tabelle, Tests, Dokumentation.
2. **Provisionierung & Lifecycle (umgesetzt)** — Tenant-Anlage erzeugt Schema und führt
   die vorhandenen `${TableScheme}`-DDL-Skripte aus; Registry-Persistenz über
   `TenantStorage`-Tabelle; Rollback der Tenant-Anlage bei Provisionierungsfehlern;
   Offen: Migrations-Controller-Iteration über alle Tenant-Schemata, Backup/Restore
   pro Tenant.
3. **Request-Pfad absichern** — `GetTableScheme()` dynamisch über
   `COperationContext`-TenantId + Resolver; Guard-Schicht (fehlende/fremde TenantId →
   Fehler); Cross-Tenant-Pfade (CrossOrgGrants, DelegatedAccess) explizit über den
   Shared-Katalog; Row-Level Security auf Shared-Tabellen.
4. **Datenmigration** — pro Tenant Kopieren aus Shared-Tabellen in Tenant-Schema,
   Verifikation (Zeilenzahlen/Checksummen), Dual-Read-Übergangsmodus per Feature-Flag;
   dateibasierte Dokumente nach `<root>/tenants/<tenantId>/...`.
5. **Härtung** — optionale Verschlüsselung pro Tenant (Postgres Tablespace-/Disk-Ebene,
   verschlüsselte SQLite-Dateien) für Enterprise-Tenants; Lasttests mit vielen Schemata.

## CRA-Mapping (Kurzfassung)

| CRA-Anforderung (Anhang I) | Umsetzung |
|---|---|
| Zugriffskontrolle, Schutz vor unbefugtem Zugriff (2(d)) | Physische Trennung pro Tenant + fail-closed Resolver + RLS |
| Vertraulichkeit/Integrität (2(e), (f)) | Getrennte Storage-Bereiche, begrenzter Blast-Radius, optionale Verschlüsselung (Phase 5) |
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
