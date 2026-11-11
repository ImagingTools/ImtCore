# StorageLink

`imtbase::StorageLink<T>` is a typed link to an object stored somewhere else (object collection, memory, database, external storage).
The data model keeps only the link; the way of storing is chosen at run time by a storage resolver.

## Parts

| Type | Role |
|---|---|
| `StorageLink<T>` / `StorageLinkBase` | The link. Serializes only the object ID and the logical storage ID. `->` returns the object, loading it on demand. |
| `IStorageResolver` | Loads (also several at once), saves, inserts and removes objects by ID and reports storage parameters. |
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
| `CollectionStorageResolver` (`CCollectionStorageResolverComp`) | `Collection`: the collection used as the storage. `TypeId`: the type-ID of the objects created by `InsertObject`. `VersionInfo`: the version info of the application, for the change detection of `Save()` (see [Saving](#saving)). |
| `StorageResolverRegistry` (`CStorageResolverRegistryComp`) | `StorageIds` and `StorageResolvers`: the resolver at a position of the list serves the storage ID at the same position; an empty ID is the default resolver. `IsDefaultProvider`: the registry becomes the provider of all links without own binding while the component exists. |

Only one registry of a process should have `IsDefaultProvider` enabled. On destruction the registry clears the default provider only if it is still the registered one.

## Links in data classes

`StorageLinkBase::Serialize` writes its own `ObjectId` and `StorageId` tags. A data class that already stores the ID of a referenced object keeps the ID member and its archive tag, so the stored documents stay readable, and builds the link in its getter:

```cpp
imtbase::StorageLink<const imtgeo::IAddressElementInfo> CDevice::GetAddress() const
{
	return imtbase::StorageLink<const imtgeo::IAddressElementInfo>(m_addressId, "Address");
}
```

Link the read-only interface (`const T`) when the caller follows a reference. The referenced object is loaded by each link separately, so keep the link in a local variable when it is used more than once.

## Saving

`Save()` never writes an unchanged object. Every write loads the storage, and a storage with revisions gets a new revision on every write. No collection skips unchanged data by itself.

A link to a snapshot storage keeps the state of the object as it was loaded (or last saved) and compares the object with it:

1. By serialization, if the object is `iser::ISerializable` and the resolver has version info (`IStorageResolver::GetVersionInfo`, the `VersionInfo` reference of the resolver component). This compares exactly the persisted data. Without the version info the data of newer versions would be skipped, so it is not used.
2. Otherwise by `IsEqual` of a clone, if the class declares `SO_CLONE` and `SO_COMPARE`.
3. Otherwise the change can't be detected: `Save()` doesn't write and returns `false`.

An object set by `operator=` before any loading is compared with the stored object.

A read-only link (`StorageLink<const T>`) can't change its object: it keeps no state at loading and its `Save()` never writes.

`Save()` of a missing object fails and doesn't create it. Collections differ here: in-memory, file and remote collections fail, a SQL table delegate reports success for an `UPDATE` of no rows, and the JSON document delegate may insert a row. The collection resolver therefore checks the type-ID first. In a database that reads the row, but only a changed object gets that far.

## Lists

Each link loads its object with its own storage access: a list of N links makes N queries. Load the links of a list together before accessing them:

```cpp
QList<imtbase::StorageLink<const imtgeo::IAddressElementInfo>> addresses;
for (const CDevice& device : devices){
	addresses.append(device.GetAddress());
}

imtbase::StorageLinkBase::LoadAll(addresses);
```

`LoadAll` asks each resolver once for all its objects (`IStorageResolver::LoadObjects`). Links to the same object loaded together share the loaded instance, as copies of a link do.

How many accesses that takes is up to the resolver: a resolver of a storage that can read several objects at once implements `LoadObjects` that way. The collection resolver works with any `IObjectCollection` (in-memory, database, file, remote), which has no access to several objects by their IDs, so it loads them one by one.

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
- Access through a link may be slow (database, network). Load the links of a list with `StorageLinkBase::LoadAll()`.
- Removing an object does not update other links to it: they stay dangling and resolve to nothing.

## Tests

`Include/imtbase/Test/CStorageLinkTest.cpp`.
