# Exclusive document access (imtdoc)

`imtdoc::IDocumentService` allows a client to modify a stored document **in place**
through an exclusive pointer. Acquiring the pointer and taking the lock is a single
operation: the returned handle *is* the lock.

## API

```cpp
typedef std::shared_ptr<istd::IChangeable> ExclusiveDocumentPtr;

ExclusiveDocumentPtr LockDocumentForEdit(
            const QByteArray& userId,
            const QByteArray& documentId,
            int waitTimeoutMs = 0,
            OperationStatus* statusPtr = nullptr);
```

* `waitTimeoutMs == 0` — do not wait, fail immediately with `OS_DOCUMENT_LOCKED`.
* `waitTimeoutMs > 0` — wait at most this number of milliseconds.
* `waitTimeoutMs < 0` — wait indefinitely.

An empty handle is returned on failure; the reason is reported in `statusPtr`
(`OS_INVALID_USER_ID`, `OS_INVALID_DOCUMENT_ID`, `OS_DOCUMENT_LOADING`,
`OS_DOCUMENT_LOCKED`, `OS_FAILED`).

## Usage

```cpp
imtdoc::IDocumentService::OperationStatus status;
auto documentPtr = documentServicePtr->LockDocumentForEdit(userId, documentId, 0, &status);
if (!documentPtr){
    return status;
}

CMyDocument* myDocumentPtr = dynamic_cast<CMyDocument*>(documentPtr.get());
myDocumentPtr->SetSomething(value);

// The lock is released here, when the last copy of the handle is destroyed -
// also on an early return or while an exception is propagated.
```

## Semantics

* Holding the handle means holding the lock. Copying the handle prolongs the
  lock, so several places of the same client may share it; a `std::weak_ptr`
  created from it does **not** hold the lock.
* A second `LockDocumentForEdit` call of the same user returns the very same
  handle instead of taking a nested lock.
* The handle does not own the document data — its deleter only releases the
  lock inside the service.
* In single-copy mode the lock belongs to the shared collection object, so it
  is effective for all user sessions working on the same object.

## What is blocked

While the lock is held, operations of **other** clients fail with
`OS_DOCUMENT_LOCKED`:

* `SetDocumentData`, `SetDocumentName`,
* `GetDocumentUndoManager` (and therefore undo/redo),
* saving the document (`TT_SAVE`).

Closing the document (`TT_CLOSE`) is rejected for **every** client, including
the lock owner, because the issued pointer would otherwise become dangling.

Reading is never blocked: `GetDocumentPtr`, `GetDocumentData` and
`GetOpenedDocumentList` keep working. The list items report the lock state via
`DocumentInfo::isLockedForEdit` and `DocumentInfo::lockOwnerUserId`.

## Undo and notifications

A change group is opened when the lock is taken and closed when it is released,
so all modifications done through the handle form a single undo step and result
in one change notification to the registered observers and document service
event handlers.

## Remote clients

The mechanism is server-side only: no pointer leaves the process and the GraphQL
schema (`Sdl/imtbase/1.0/CollectionDocumentService.sdl`) is not affected.
