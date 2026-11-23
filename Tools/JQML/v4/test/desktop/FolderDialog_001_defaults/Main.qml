import QtQuick
import Qt.labs.platform 1.1

// FolderDialog (PathPicker, FilePickerElementView) without opening it: the properties it has, what they hold, and that they can be written. The
// engine's FileDialog is the model for the browser side (title, folder, currentFolder, visible, open(), close(), accepted, rejected).
Item {
    width: 40
    height: 40

    FolderDialog {
        id: dialog
        title: "Choose a folder"
        folder: "file:///C:/Work"
        onAccepted: console.log("accepted")
        onRejected: console.log("rejected")
    }
    FolderDialog { id: empty }

    Component.onCompleted: {
        console.log("visible", dialog.visible, empty.visible)
        console.log("title", dialog.title, "[" + empty.title + "]")
        console.log("folder", dialog.folder.toString(), typeof dialog.folder.toString())
        console.log("replace", dialog.folder.toString().replace("file:///", ""))
        console.log("methods", typeof dialog.open, typeof dialog.close)
        dialog.folder = "file:///D:/Data"
        console.log("written", dialog.folder.toString())
        console.log("replace again", dialog.folder.toString().replace("file:///", ""))
        Qt.quit()
    }
}
