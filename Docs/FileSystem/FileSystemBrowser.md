# File System Browser

The File System Browser provides a QML navigation UI for selecting folders (and
optionally files/executables). The folder hierarchy is delivered by a single
server-side GraphQL controller. The underlying storage may be local (`QDir`) or
remote (forwarded through `IGqlClient`) — that choice is made by which
implementation of `IFileSystemStructureProvider` is wired into the controller.

## Architecture

```
QML Web Client                         Server
--------------                         ------
ServerPathPicker /                     CFileSystemControllerComp  (permissible,
  ServerPathPickerElementView            SDL-generated CFileSystemGqlHandlerCompBase)
  └─ FileSystemBrowserDialog   GQL →     └─ IFileSystemStructureProvider
       └─ GqlRequestSender (+headers)         ├─ CFileSystemStructureProviderComp
                                              │    (local, QDir; hard RootPath sandbox,
                                              │     or whole machine + drives root
                                              │     via AllowWholeFileSystem)
                                              └─ CRemoteFileSystemStructureProviderComp
                                                   (forwards via IGqlClient + IUrlParam)
```

**DIP:** remoteness lives behind the domain interface. There is one GQL
controller; local vs remote is a provider implementation detail.

## GraphQL API

Schema: `Sdl/imtbase/1.0/FileSystem.sdl` (SDL is the source of truth; codegen is used).

Query `GetFileSystemEntries(input: { path, offset, limit, nameFilter, sortBy,
sortAscending })` returns:

| Field | Description |
|-------|-------------|
| `path` | Canonical path of the browsed folder |
| `parentPath` | Parent folder path (empty at configured root) |
| `entries` | List of `FileSystemEntry` |
| `totalCount` | Total number of entries before pagination |
| `hasMore` | Whether more entries exist after this page |

`FileSystemEntry` fields: `name`, `path`, `entryType` (`File` \| `Dir` \| `Symlink` \| `Drive`),
`size`, `lastModified`, plus `totalBytes` / `freeBytes` (`Double`, `Drive` entries only —
a volume's byte count overflows the 32-bit `size`).

Empty `input.path` resolves to the configured hard root (`RootPath`), or to the
virtual drives root when `AllowWholeFileSystem` is set. **Read-only** — no
create/rename/delete mutations, and file *contents* are never exposed.

`parentPath` uses **presence, not emptiness**, to signal that a parent exists:
absent means "nothing above"; present-and-empty addresses the virtual drives
root (the parent of `C:/`). Collapsing the two would make `Up` unusable from a
volume root.

## Domain (`imtfile`)

### `IFileSystemStructureProvider`

Result-object API (no multi-out-parameter style):

- `GetEntries(FileSystemQuery, FileSystemListing&, QString& error)`
- `Stat(path, FileSystemEntry&, QString& error)`

### `CFileSystemStructureProviderComp`

Local `QDir` implementation. Attributes:

- `RootPath` — single browsable root (hard sandbox via canonical paths).
  - `${DriveRoot}` or empty → `QDir::rootPath()` (e.g. `C:/` on Windows) — default for agents
    so service executables outside the agent install tree remain selectable.
  - `${ApplicationDir}` → process directory (tighter sandbox when desired).
  - Absolute paths are used as-is. Still **one** root (no multi-drive enumeration).
  - **Ignored** when `AllowWholeFileSystem` is `true`.
- `AllowWholeFileSystem` — **security-relevant**, default `false`. See below.
- `ShowFiles` — include files in listings
- `ShowHidden` — include hidden entries

Sandbox is centralized in `NormalizeAndAuthorize` (symlinks / `..` / absolute
paths outside root are denied).

#### Whole-file-system mode (`AllowWholeFileSystem`)

Turns the provider from "one subtree" into "the whole machine":

- `RootPath` is ignored and **no containment check is applied** — any absolute
  path the agent process can read is listable.
- An empty path lists the storage volumes (`QDir::drives()`) as a **virtual
  drives root**: `path` comes back empty, entries are `Drive`, nothing above it.
  A volume root (`C:/`, `/`) reports that virtual root as its parent.
- `Drive` entries are named from `QStorageInfo::displayName()` plus the volume
  root key when it is not already in the label (Windows Explorer style:
  `Local Disk (C:)` / `Data (D:)` — a bare volume label alone is ambiguous when
  several disks share the same name). A volume root's `QFileInfo::fileName()`
  is empty, so naming is never taken from that. Entries also carry
  `totalBytes` / `freeBytes`.
- Uniform across platforms: on Unix `QDir::drives()` yields a single `/`.

The mode is **server-side configuration only** — the client sends just a `path`
and cannot request it, so the sandbox can never be lifted from the GUI. Scope is
whatever the agent process may read; only metadata (names/sizes/dates) is
exposed, never file contents.

> A `Drive` behaves as a container: every `Dir` branch must accept it too, or
> drives become non-navigable leaves.

**Permission:** `GetFileSystemEntries` is gated by `CPermissibleGqlRequestHandlerComp`.
In Agentino configs it requires the existing `ChangeService` permission (browse is
part of service configuration, not a free read for every authenticated user).

**Size field:** GraphQL `size` is SDL `Int` (32-bit). Values above `INT_MAX` are
clamped when mapped from domain `qint64` — fine for exe selection; not for archival
file inventories.

**Server filter / sort:** applied in `CFileSystemStructureProviderComp` **before**
pagination, so `totalCount` / `hasMore` describe the filtered set and every page
uses the same order.

| Input field | Meaning |
|-------------|--------|
| `nameFilter` | Case-insensitive name filter on entry display name (optional). Default is shell-style glob (`*` / `?`; other characters literal — e.g. `*.exe`). Full PCRE regex when wrapped as `/pattern/`. Invalid patterns fall back to literal substring match. |
| `extensionFilter` | `;`-separated file extensions to include (no dot, e.g. `exe;dll`); empty = all. Token `*` (or `none`) also allows extensionless names (Linux binaries). Files only — folders always listed so navigation is never blocked. Case-insensitive |
| `sortBy` | `Name` (default) \| `Date` \| `Size` (`FileSystemSortBy`) |
| `sortAscending` | Default `true` (A→Z / oldest / smallest) |

`extensionFilter` travels the wire as one `;`-separated string (list-typed GraphQL
inputs are awkward to build from the web client); the controller splits it into
the domain `FileSystemQuery::extensions` list, and the provider's
`ApplyExtensionFilter` drops non-matching **files** while always keeping
containers. The QML `ServerPathPicker` exposes it as an `extensions` string
array (e.g. `["exe"]`) and joins it before opening the dialog.

Containers (dir / symlink / drive) always sort above files. For drives,
`Date` sorts by `freeBytes` and `Size` by `totalBytes` (same as the UI columns
at the drives root). The QML dialog sends these fields on every
`GetFileSystemEntries` request (including “Load more”) and re-queries page 0
when the filter or sort changes.

### `CRemoteFileSystemStructureProviderComp` (`imtclientgql`)

Implements the same domain interface by forwarding `GetFileSystemEntries` through
`IGqlClient` (`ApiClient`). Optional `UrlParam` is passed to `SendRequest`.
Response parsing uses generated SDL types.

## Server controller (`imtservergql`)

### `CFileSystemControllerComp`

- Inherits generated `sdl::V1_0::imtbase::CFileSystemGqlHandlerCompBase`
  (which already extends `CPermissibleGqlRequestHandlerComp`)
- Depends only on `IFileSystemStructureProvider` (`FileSystemProvider` attribute)
- Builds the payload via generated types (`WriteToJsonObject`)
- Permission: register command id `GetFileSystemEntries` in
  `CommandPermissions` with a suitable permission id (e.g. `BrowseFileSystem`)

Register the controller on the GraphQL servlet demultiplexer like any other handler.

## QML (`Qml/imtguigql`)

### `ServerPathPicker`

Text field + Browse button. Properties:

- `path`, `pathKind` (`dir` \| `file` \| `executable`)
- `extensions` — optional file-extension whitelist (no dots, e.g. `["exe", "dll"]`);
  empty = all files. Files only; folders stay navigable
- `headersProvider` — function returning request headers (e.g. `{ clientid: agentId }`)
- `browseEnabled` — when false, only manual path entry is available
- `readOnlyPath` — path field read-only mode

### `ServerPathPickerElementView`

`ElementView` wrapper for use inside `GroupElementView` / editor layouts.

### `FileSystemBrowserDialog`

Modal browser: columns name/size/modified, name filter field, column sorting,
keyboard navigation, empty state, pagination (`offset`/`limit`/`hasMore`).
`showFiles` is server config; the dialog’s name filter and column sort are
request parameters (`nameFilter` / `sortBy` / `sortAscending`).

At the virtual drives root ("This PC") the two right-hand columns switch from
`Date modified` / `Size` — meaningless for a volume — to `Free space` /
`Total size`, rather than standing empty (and sort those capacity columns).

## Configuration examples

### Local file system

1. `FileSystemStructureProvider` (ImtFilePck) — set `RootPath`, or set
   `AllowWholeFileSystem` to expose the whole machine with a drives root
2. `FileSystemController` (ImtServerGqlPck) —
   `FileSystemProvider` → provider, `CommandIds` = `GetFileSystemEntries`,
   wire `CommandPermissions` / `PermissionChecker`
3. Register controller on the servlet demultiplexer

### Remote file system (another machine)

1. Remote host: local controller + local provider as above
2. Caller host: `RemoteFileSystemStructureProvider` (ImtClientGqlPck) with
   `ApiClient` (and optional `UrlParam`)
3. Same `FileSystemController` with `FileSystemProvider` → remote provider

### Agentino (agent-scoped browse)

Path of a service is on the **agent** machine (`Service.agentId`). Pattern:

1. **Agent** hosts `FileSystemController` + local `FileSystemStructureProvider`
2. **Agentino server** hosts `CFileSystemControllerProxyComp` which forwards
   `GetFileSystemEntries` with header `clientid = agentId` (same channel as
   `CServiceControllerProxyComp`)
3. **Client** `ServiceEditor` uses `ServerPathPickerElementView` with
   `headersProvider: () => ({ clientid: agentId })`. Browse is disabled until
   an agent is selected; manual path entry remains the fallback
