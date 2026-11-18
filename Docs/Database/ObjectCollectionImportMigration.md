# Importing objects from another collection in a migration

`imtdb::CObjectCollectionImportMigrationControllerComp` (package `ImtDatabasePck`, component `ObjectCollectionImportMigrationController`) is a migration step that copies the active objects of a source collection into a table of the migrated database.
It is meant for moving data between applications, for example a catalog that another server used to own.

## What the step does

1. Reads the IDs of the source collection. A read that reports an error fails the step, so the revision is not recorded with nothing imported.
2. Skips an object whose ID is already in the target table. A repeated step changes nothing.
3. Passes each object to the optional converter. The converter keeps the object, replaces it with an object of another type, or leaves it out.
4. Writes the object through the delegate of the target table, with the ID of the source object. The delegate creates the meta info; no organization owns the object.

The objects keep their IDs, so references to them in the migrated database stay valid without being rewritten.

## Attributes

| Attribute | Required | Meaning |
|---|---|---|
| `From`, `To` | yes | The revision of the step. |
| `DatabaseEngine` | yes | Engine of the migrated database. |
| `SourceCollection` | yes | `imtbase::IObjectCollection` the objects come from, usually an SQL collection on a second database engine. |
| `TargetDelegate` | yes | `imtdb::ISqlDatabaseObjectDelegate` of the target table. |
| `ObjectConverter` | no | `imtdb::IObjectImportConverter`. Without it every object is imported unchanged. |
| `SourceEnabler` | no | `iprm::IEnableableParam`. Switched off, the step imports nothing and passes. |

## Configuring the source

The source database is only read. Give its engine `AutoCreateDatabase = 0`, `AutoCreateTables = 0` and no `MigrationController`, so nothing is created or migrated there.

Keep the step behind a `SourceEnabler` that is off by default. An installation without the source database, such as a new installation or a test system, then still starts. Switch it on before the update of an installation that has the source.

## Converters

`IObjectImportConverter::ConvertObject` returns:

- `CR_CONVERTED` — the object is imported. If the converter sets `targetObject.objectPtr`, that object is written; otherwise the source object is written unchanged.
- `CR_SKIPPED` — the object does not belong to this target.
- `CR_FAILED` — the object belongs to this target but cannot be converted. The step fails.

A converter that needs another object to decide must fail when that object cannot be read, rather than guess.

One source collection can feed several steps with different converters, for example licenses of software products into a license table and licenses of hardware products into a configuration table.
The composite migration controller runs one controller per revision, so each step needs its own revision.
