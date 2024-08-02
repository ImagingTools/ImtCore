import QtQuick 2.0
import Qt.labs.platform 1.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: root;

    width: 300;
    height: content.height;

    Column {
        id: content;

        width: parent.width;

        spacing: Style.size_mainMargin;

        BaseText {
            text: qsTr("Before performing the operation, check the connection to the server");
        }

        Button {
            id: backupButton;

            width: 70;
            height: 25;

            text: qsTr("Backup");

            onClicked: {
                fileDialogSave.open();
            }
        }

        Button {
            id: restoreButton;

            width: 70;
            height: 25;

            text: qsTr("Restore");

            onClicked: {
                fileDialog.open();
            }
        }
    }

    RemoteFileController {
        id: remoteFileController;

        prefix: "/files/";

        onFileUploaded: {
            ModalDialogManager.openDialog(messageErrorDialog, {"title" : qsTr("Restore successful"),"message": qsTr("Database restore was successful")});
        }

        onFileUploadFailed: {
            ModalDialogManager.openDialog(messageErrorDialog, {"message": qsTr("Error when trying to restore the database")});
        }

        onFileDownloadFailed: {
            ModalDialogManager.openDialog(messageErrorDialog, {"message": qsTr("Error when trying to create a database backup")});
        }

        onStateChanged: {
            if (remoteFileController.state === "Loading"){
                Events.sendEvent("StartLoading");
            }
            else{
                Events.sendEvent("StopLoading");
            }
        }
    }

    Component {
        id: messageErrorDialog;

        ErrorDialog {}
    }

    FileDialog {
        id: fileDialog;

        title: qsTr("Select backup file");

        fileMode: FileDialog.OpenFile;

        nameFilters: ["All files (*)"];

        onAccepted: {
            let filePath = fileDialog.file.toString();
            filePath = filePath.replace('file:///', '')

            remoteFileController.SendFile(filePath);
        }
    }

    FileDialog {
        id: fileDialogSave;

        title: qsTr("Save file");

        nameFilters: ["License files (*.backup)", "All files (*)"];

        fileMode: FileDialog.SaveFile;

        onAccepted: {
            var pathDir = fileDialogSave.folder.toString();
            remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
            var fileName = fileDialogSave.file.toString().replace(pathDir + "/", '');

            remoteFileController.GetFile("GetBackupFile", fileName);
        }
    }
}


