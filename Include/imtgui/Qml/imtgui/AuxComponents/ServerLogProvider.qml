import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import QtQuick.Dialogs 1.3

Item {
    id: root;

    width: 100;
    height: 30;

    AuxButton {
        id: button;

        width: root.width;
        height: root.height;

        hasText: true;
        hasIcon: false;

        textButton: qsTr("Download");
        borderColor: (button.highlighted || button.focus) ? Style.iconColorOnSelected : Style.buttonColor;
        backgroundColor: Style.imagingToolsGradient2;

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
        selectExisting: false;
        folder: shortcuts.home;

        nameFilters: ["Text files (*.txt)", "All files (*)"];

        onAccepted: {
            var pathDir = fileDialogSave.folder.toString();
            remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
            var fileName = fileDialogSave.fileUrl.toString().replace(pathDir + "/", '');
            var id = "";

            if (fileName == ""){
                fileName = {};
                fileName["name"] = "ServerLog.txt";
            }

            remoteFileController.GetFile(id, fileName);
        }
    }
}
