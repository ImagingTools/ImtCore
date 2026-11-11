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
| `CCollectionStorageResolverComp`, `CStorageResolverRegistryComp` | Components of the collection resolver and of the registry, see [Components](#components). |

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

## Components

Applications assembled from compositions bind the links there instead of in code. Both components are in `ImtCorePck`.

| Component | Attributes and references |
|---|---|
| `CollectionStorageResolver` (`CCollectionStorageResolverComp`) | `Collection`: the collection used as the storage. `TypeId`: the type-ID of the objects created by `InsertObject`. |
| `StorageResolverRegistry` (`CStorageResolverRegistryComp`) | `StorageIds` and `StorageResolvers`: the resolver at a position of the list serves the storage ID at the same position; an empty ID is the default resolver. `IsDefaultProvider`: the registry becomes the provider of all links without own binding while the component exists. |

Only one registry of a process should have `IsDefaultProvider` enabled. On destruction the registry clears the default provider only if it is still the registered one.

## Related types

- `ICollectionObjectLink` / `CObjectLink` keep the object ID together with a pointer to the collection instance and resolve against that instance only. There is no late binding.
- `IReferenceCollection` keeps references by object ID and repository ID. The repository ID has the role of the logical storage ID, but it is not resolved through `IStorageResolverProvider`.
- `CObjectCollectionBase` follows its own external links to the storages configured in the collection component.

`StorageLink<T>` is the type for new data models; the types above are not changed by it.

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
