# CCollectionDocumentServiceComp: state file persistence

`imtdoc::CCollectionDocumentServiceComp` can save and restore the list of opened
documents to/from a JSON file.

## Automatic persistence

State-file loading and saving are handled internally by the component. The
`LoadFromFile` and `SaveToFile` helpers are private and are not a public API.
The state file is written atomically via `QSaveFile`, and missing directories
are created.

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
