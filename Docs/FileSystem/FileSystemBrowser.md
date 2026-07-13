# File System Browser

Der File System Browser stellt im Web Client (QML) eine Navigations-UI zur Auswahl
von Ordnern (und optional Dateien) bereit. Die Ordner-Hierarchie wird von einem
serverseitigen File System Controller über GraphQL geliefert. Das File System kann
lokal auf dem Server liegen oder von einem File Service auf einer anderen Maschine
bereitgestellt werden.

## Architektur

```
QML Web Client                        Server                          Remote File Service
--------------                        ------                          -------------------
ServerPathPicker                      CFileSystemControllerComp
  └─ FileSystemBrowserDialog   GQL →    └─ IFileSystemStructureProvider
       └─ GqlRequestSender                   └─ CFileSystemStructureProviderComp (lokal, QDir)

                                      (alternativ)
                                      CRemoteFileSystemControllerComp   GQL →   CFileSystemControllerComp
                                        └─ IGqlClient (ApiClient)                 └─ CFileSystemStructureProviderComp
```

## GraphQL API

Schema: `Sdl/imtbase/1.0/FileSystem.sdl`

Query `GetFileSystemEntries(input: { path: String })` liefert:

- `path` – normalisierter Pfad des durchsuchten Ordners
- `parentPath` – Pfad des übergeordneten Ordners (leer am Root)
- `entries` – Liste von `FileSystemEntry` (`name`, `path`, `isDir`, `size`, `lastModified`)

Ist `input.path` leer, wird der konfigurierte Root-Ordner geliefert.

## Server-Komponenten

### `imtfile::IFileSystemStructureProvider`

Schnittstelle für die Bereitstellung der Ordner-Hierarchie
(`Include/imtfile/IFileSystemStructureProvider.h`).

### `imtfile::CFileSystemStructureProviderComp`

Lokale Implementierung auf Basis von `QDir`
(`Include/imtfile/CFileSystemStructureProviderComp.*`).

Attribute:

- `RootPath` – Root-Ordner des durchsuchbaren File Systems. Das Browsen ist auf
  diesen Ordner beschränkt (Schutz vor Path-Traversal über kanonische Pfade).
- `ShowFiles` – wenn `true`, werden zusätzlich zu Ordnern auch Dateien geliefert.
- `ShowHidden` – wenn `true`, werden versteckte Einträge geliefert.

### `imtservergql::CFileSystemControllerComp`

GraphQL-Controller für die Command-ID `GetFileSystemEntries`
(`Include/imtservergql/CFileSystemControllerComp.*`). Wird wie jeder andere
GQL-Handler am `GqlRequestHandler`-Multi-Ref des GraphQL-Servlets registriert
(`CommandIds` = `GetFileSystemEntries`). Der File-System-Zugriff wird an die
Referenz `FileSystemProvider` (`IFileSystemStructureProvider`) delegiert.

### `imtclientgql::CRemoteFileSystemControllerComp`

Umleitungs-Komponente für ein File System auf einer anderen Maschine
(`Include/imtclientgql/CRemoteFileSystemControllerComp.*`). Sie implementiert
denselben `IGqlRequestHandler`-Vertrag und wird anstelle des lokalen Controllers
am Servlet registriert. Eingehende `GetFileSystemEntries`-Requests werden über
die Referenz `ApiClient` (`IGqlClient`) an den entfernten File Service
weitergeleitet und die Antwort an den Aufrufer zurückgegeben.

## QML-Komponenten (`Qml/imtguigql`)

### `ServerPathPicker`

Textfeld mit Browse-Button. Öffnet den `FileSystemBrowserDialog` über den
`ModalDialogManager` und übernimmt den ausgewählten Pfad in die Eigenschaft `path`.

```qml
ServerPathPicker {
    onPathChanged: {
        console.log("Selected path:", path);
    }
}
```

### `FileSystemBrowserDialog`

Modaler Dialog mit Navigations-UI: aktueller Pfad, "Up"-Button, Liste der
Ordner/Dateien (Doppelklick öffnet einen Ordner, Klick wählt einen Eintrag aus).
Die Daten werden per `GqlRequestSender` mit der Command-ID `GetFileSystemEntries`
geladen.

Eigenschaften: `startPath`, `showFiles`; Signal: `pathSelected(selectedPath)`.

## Konfiguration (Beispiel)

Lokales File System:

- `FileSystemController` (ImtServerGqlPck) mit `CommandIds = GetFileSystemEntries`
  und `FileSystemProvider` → `FileSystemStructureProvider` (ImtFilePck, `RootPath` setzen);
  Registrierung am `GqlRequestHandler`-Multi-Ref des GraphQL-Servlets.

File System einer anderen Maschine:

- `RemoteFileSystemController` (ImtClientGqlPck) mit `CommandIds = GetFileSystemEntries`
  und `ApiClient` → GQL-Client, der mit dem entfernten File Service verbunden ist;
  Registrierung am `GqlRequestHandler`-Multi-Ref des GraphQL-Servlets.
