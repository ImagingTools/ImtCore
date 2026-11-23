import QtQuick
import QtCore

// StandardPaths of QtCore: the location types; nothing found for a name no place has
Item {
    Component.onCompleted: {
        console.log(StandardPaths.DesktopLocation, StandardPaths.DocumentsLocation, StandardPaths.HomeLocation, StandardPaths.AppDataLocation, StandardPaths.GenericStateLocation, StandardPaths.LocateFile, StandardPaths.LocateDirectory)
        console.log(String(StandardPaths.locate(StandardPaths.DocumentsLocation, "nonexistent-xyz.txt")) === "", JSON.stringify(StandardPaths.locateAll(StandardPaths.DocumentsLocation, "nonexistent-xyz.txt")), String(StandardPaths.findExecutable("nonexistent-xyz")) === "")
        Qt.quit()
    }
}
