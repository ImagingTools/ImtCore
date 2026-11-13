# Document service: state file persistence

`imtdoc::CDocumentServiceCompBase` (and therefore every document service component derived
from it, e.g. `CDocumentServiceComp` and `CCollectionDocumentServiceComp`) can save and restore
the list of opened documents to/from a JSON file.

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

The file contains the descriptions of the opened documents and, separately, the IDs of
the documents opened by each user. A document shared by several users (single-copy mode)
is described once. Document data itself is **not** stored. On restore the data object is
created by the virtual method `CreateObjectFromSourceRef(typeId, documentSourceRef)`, which
is implemented by storage-specific subclasses; if it fails (no source reference, the source is
not available, or the subclass does not implement it), the data object is created by
`CreateObject(typeId)` as a new empty document. Unsaved changes are restored by a persistent
undo manager (`IPersistentUndoManager`), if any.

`LoadFromFile()` validates the JSON strictly:
- the root object must contain the arrays `Documents` and `Users`;
- each document object must contain `DocumentId` (non-empty, unique), `TypeId`
  (a document with an empty `TypeId` is not restored),
  `DocumentSourceRef`, `Name`, `IsReadOnly` and `SingleDocumentInstance`;
- each user object must contain `UserId` and the array `DocumentIds`; every ID must
  reference an object in `Documents`.

If the state file is missing, cannot be read or has invalid content, the persisted data of
all documents can no longer be associated to any document. In this case the component calls
`IPersistentUndoManager::RemoveAllDocumentContexts()`, which removes all document folders
(subdirectories of `RootFolder` for `CFileBasedUndoManagerComp`; files directly in
`RootFolder` are kept).

`DocumentSourceRef` is stored as is; its format is defined by the service implementation
and it is empty for a document that was never saved. `IsReadOnly` is restored as is.
In `CCollectionDocumentServiceComp` the source reference is `<objectId>` or
`<objectId>/<revision>`: the data is loaded from the collection object or its revision,
and a document opened from a revision stays read-only.

```json
{
    "Documents": [
        {
            "DocumentId": "doc-1",
            "TypeId": "Contact",
            "DocumentSourceRef": "obj-1",
            "Name": "Document name",
            "IsReadOnly": false,
            "SingleDocumentInstance": false
        },
        {
            "DocumentId": "doc-2",
            "TypeId": "Contact",
            "DocumentSourceRef": "obj-1/3",
            "Name": "Document name (Rev #3)",
            "IsReadOnly": true,
            "SingleDocumentInstance": false
        }
    ],
    "Users": [
        {
            "UserId": "user-1",
            "DocumentIds": ["doc-1", "doc-2"]
        },
        {
            "UserId": "user-2",
            "DocumentIds": ["doc-1"]
        }
    ]
}
```
