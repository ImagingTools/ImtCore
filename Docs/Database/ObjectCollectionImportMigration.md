# Importing objects from another collection in a migration

`imtdb::CObjectCollectionImportMigrationControllerComp` (package `ImtDatabasePck`, component `ObjectCollectionImportMigrationController`) is a migration step that copies the active objects of a source collection into a table of the migrated database.
It is meant for moving data between applications, for example a catalog that another server used to own.

## What the step does

1. Checks the source database (see `SourceDatabase` below): without it there is nothing to import.
2. Reads the IDs of the source collection. A read that reports an error fails the step, so the revision is not recorded with nothing imported.
3. Skips an object whose ID is already in the target table. A repeated step changes nothing.
4. Passes each object to the optional converter. The converter keeps the object, replaces it with an object of another type, or leaves it out.
5. Writes the object through the delegate of the target table, with the ID of the source object. The delegate creates the meta info; no organization owns the object.

The objects keep their IDs, so references to them in the migrated database stay valid without being rewritten.

## Attributes

| Attribute | Required | Meaning |
|---|---|---|
| `From`, `To` | yes | The revision of the step. |
| `DatabaseEngine` | yes | Engine of the migrated database. |
| `SourceCollection` | yes | `imtbase::IObjectCollection` the objects come from, usually an SQL collection on a second database engine. |
| `TargetDelegate` | yes | `imtdb::ISqlDatabaseObjectDelegate` of the target table. |
| `ObjectConverter` | no | `imtdb::IObjectImportConverter`. Without it every object is imported unchanged. |
| `SourceDatabase` | no | `imtdb::IDatabaseServerConnectionChecker` of the database the source collection lives in, usually its engine. If that database does not exist, the step imports nothing and passes. |

## Configuring the source

The source database is only read. Give its engine `AutoCreateDatabase = 0`, `AutoCreateTables = 0` and no `MigrationController`, so nothing is created or migrated there.

Give the step the engine of the source as `SourceDatabase`. The step then decides by the source database itself:

- the database does not exist (a new installation, a test system): there is nothing to import, the step passes;
- the database exists: the objects are imported;
- the database cannot be checked (the server is not reachable, the login fails) or the source cannot be read: the step fails. The revision is not recorded, so the import runs again at the next start once the cause is fixed.

Do not put the step behind a switch that is off by default: a step that imports nothing still records its revision, and an installation started once with the switch off can never import its data.

## Converters

`IObjectImportConverter::ConvertObject` returns:

- `CR_CONVERTED` — the object is imported. If the converter sets `targetObject.objectPtr`, that object is written; otherwise the source object is written unchanged.
- `CR_SKIPPED` — the object does not belong to this target.
- `CR_FAILED` — the object belongs to this target but cannot be converted. The step fails.

A converter that needs another object to decide must fail when that object cannot be read, rather than guess.

One source collection can feed several steps with different converters, for example licenses of software products into a license table and licenses of hardware products into a configuration table.
The composite migration controller runs one controller per revision, so each step needs its own revision.
