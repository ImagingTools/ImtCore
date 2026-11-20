# StorageLink

`imtbase::StorageLink<T>` is a typed link to an object stored somewhere else (object collection, memory, database, external storage).
The data model keeps only the link; the way of storing is chosen at run time by a storage resolver.

## Parts

| Type | Role |
|---|---|
| `StorageLink<T>` / `StorageLinkBase` | The link. Serializes only the object ID and the logical storage ID. `->` returns the object, loading it on demand. |
| `IStorageResolver` | Loads, saves, inserts and removes objects by ID and reports storage parameters. |
| `IStorageResolverProvider` | Returns a resolver by the logical storage ID. |
| `CStorageResolverRegistry` | Provider implementation filled by the application. |
| `CMemoryStorageResolver` | Objects in memory. Live instances: changes are visible to all links at once. |
| `CCollectionStorageResolver` | Adapter over `IObjectCollection`. Snapshot semantic: call `Save()` to write changes back. |

## Late binding

A link finds its resolver in this order:

1. `SetResolver()` on the link.
2. `SetResolverProvider()` on the link.
3. The application-wide provider set with `StorageLinkBase::SetDefaultResolverProvider()`.

An empty storage ID selects the default resolver of the provider. An unknown non-empty storage ID is not resolved at all.
The resolver binding is run-time state: it is not serialized and must be restored after loading.

```cpp
imtbase::CObjectCollection addresses;
imtbase::CCollectionStorageResolver addressResolver(addresses, "Address");

imtbase::CStorageResolverRegistry registry;
registry.RegisterResolver(&addressResolver);
imtbase::StorageLinkBase::SetDefaultResolverProvider(&registry);

imtbase::StorageLink<CAddress> link(addressId);
if (link){
	link->SetCity("Berlin");
	link.Save();
}
```

Different application profiles register different resolvers for the same storage IDs; the data model does not change.

## Optional-like interface

`StorageLink<T>` follows `std::optional<T>`:

| Member | Meaning |
|---|---|
| `has_value()`, `explicit operator bool` | The linked object is available. Loads it on the first call. |
| `value()` | The object; throws `std::bad_optional_access` if it is not available. |
| `value_or(default)` | A copy of the object or of the default value. Requires `T` to be copy constructible. |
| `operator*`, `operator->` | Unchecked access (asserts in debug). Use after `has_value()`. |
| `reset()`, `= std::nullopt` | Clears the link target. The stored object stays in the storage; use `Remove()` to delete it. |
| `IsNull()` | The link has no object ID. Cheap, never loads. |

Differences to `std::optional`: copies of a link refer to the same stored object, and a link with an ID of a missing object is not null but has no value.

## Rules

- Set the default provider once at the application start, before links are used from other threads. The provider and resolvers must outlive all links.
- A single link instance is not thread-safe. Copies of a link share the loaded object.
- Check `GetResolver()` or `GetObject().IsValid()` where the binding is not guaranteed; `operator->` asserts that the object exists.
- Access through a link may be slow (database, network). Load explicitly with `GetObject()` in loops.
- Removing an object does not update other links to it: they stay dangling and resolve to nothing.

## Tests

`Include/imtbase/Test/CStorageLinkTest.cpp`.
