# CCollectionDocumentServiceComp: state file persistence

`imtdoc::CCollectionDocumentServiceComp` can save and restore the list of opened
documents to/from a JSON file.

## API

- `bool SaveToFile(const QString& filePath) const` — writes the list of opened
  documents to `filePath` (atomically, via `QSaveFile`). Missing directories are created.
- `bool LoadFromFile(const QString& filePath)` — reads `filePath`, replaces all currently
  opened documents with the documents described in the file and reloads their data
  from the `Collection`. If the file cannot be read or parsed, the current state is
  left unchanged.

## Component configuration

| Attribute | Type | Description |
|---|---|---|
| `StateFilePath` | `ifile::IFileNameParam` (optional) | Path of the state file. |
| `StatePersistenceEnabler` | `iprm::IEnableableParam` (optional) | When enabled, the state is restored from `StateFilePath` on component creation (if the file exists) and saved to it after each change of the service data model (`OnEndChanges`: document created/opened/closed/saved/renamed). Changes caused by `LoadFromFile` itself are not written back. |

## File content

The file contains the same information as `Serialize()`: user IDs and the metadata of
their opened documents. Document data itself is **not** stored; it is loaded from the
collection on restore (documents that were never saved are restored as new empty documents).

`LoadFromFile()` validates the JSON strictly:
- the root object must contain `Users` and it must be an array;
- each user object must contain both `UserId` and `Documents`;
- each document object must contain `DocumentId`, `ObjectId`, `TypeId`, `Url`, `Name`,
  and `SingleDocumentInstance`.

```json
{
    "Users": [
        {
            "UserId": "user-1",
            "Documents": [
                {
                    "DocumentId": "doc-1",
                    "ObjectId": "obj-1",
                    "TypeId": "Contact",
                    "Url": "...",
                    "Name": "Document name",
                    "SingleDocumentInstance": false
                }
            ]
        }
    ]
}
```
