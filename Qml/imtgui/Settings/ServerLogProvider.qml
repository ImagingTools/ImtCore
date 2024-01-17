import QtQuick 2.12
import QtQuick.Dialogs 1.3
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import Qt.labs.platform 1.1

Item {
    id: root;

    width: 100;
    height: 30;

    Button {
        id: button;

        width: root.width;
        height: root.height;

        text: qsTr("Download");
//        borderColor: (button.highlighted || button.focus) ? Style.iconColorOnSelected : Style.buttonColor;
//        backgroundColor: Style.imagingToolsGradient2;

        onClicked: {
            fileDialogSave.open();
        }
    }

    RemoteFileController {
        id: remoteFileController;

        prefix: "/files";
    }

    FileDialog {
        id: fileDialogSave;

        title: qsTr("Save file");

        fileMode: FileDialog.SaveFile;

        currentFile: "file:///Log.txt";

        nameFilters: ["Text files (*.txt)", "All files (*)"];

        onAccepted: {
            var pathDir = fileDialogSave.folder.toString();
            remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
            var fileName = fileDialogSave.file.toString().replace(pathDir + "/", '');
            var id = "GetServerLog";

            if (fileName == ""){
                fileName = {};
                fileName["name"] = "ServerLog.txt";
            }

            remoteFileController.GetFile(id, fileName);
        }
    }
}
