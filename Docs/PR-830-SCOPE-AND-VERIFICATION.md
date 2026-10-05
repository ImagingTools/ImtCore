# PR 830 scope and verification update

This change set is **not** limited to subscription-routing.
It includes the following behavior areas and should be reviewed/validated as such:

1. Persistent undo manager and snapshot persistence lifecycle.
2. Document-session serialization/restore flow.
3. Single-instance document open semantics.
4. Automatic idle document closing.
5. Gallery/test wiring updates for the new behavior.

## Verification plan

- [ ] Subscription routing: validate `OnDocumentChanged` filtering by document id.
- [ ] Persistent undo: restart flow keeps/restores referenced snapshot files.
- [ ] Session restore: reopen restores document and undo context correctly.
- [ ] Single-instance open: concurrent same-user/object open is rejected.
- [ ] Idle close: active keep-alive subscription prevents unintended close.
- [ ] Wiring: gallery and related tests pass with new component bindings.
