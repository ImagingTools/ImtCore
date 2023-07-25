import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import Qt.labs.platform 1.0

Item {
    id: root;

    width: 300;
    height: content.height;

    Column {
        id: content;

        width: parent.width;

        spacing: 10;

        AuxButton {
            id: backupButton;

            width: 70;
            height: 25;

            hasText: true;
            hasIcon: false;

            textButton: qsTr("Backup");
            borderColor: (backupButton.highlighted || backupButton.focus) ? Style.iconColorOnSelected : Style.buttonColor;
            backgroundColor: Style.imagingToolsGradient2;

            onClicked: {
                fileDialogSave.open();
            }
        }

        AuxButton {
            id: restoreButton;

            width: 70;
            height: 25;

            hasText: true;
            hasIcon: false;

            textButton: qsTr("Restore");
            borderColor: (restoreButton.highlighted || restoreButton.focus) ? Style.iconColorOnSelected : Style.buttonColor;
            backgroundColor: Style.imagingToolsGradient2;

            onClicked: {
                fileDialog.open();
            }
        }
    }

    RemoteFileController {
        id: remoteFileController;

        prefix: "/files";

        onFileUploaded: {
            modalDialogManager.openDialog(messageErrorDialog, {"title" : qsTr("Restore successful"),"message": qsTr("Database restore was successful")});
        }

        onFileUploadFailed: {
            modalDialogManager.openDialog(messageErrorDialog, {"message": qsTr("Error when trying to restore the database")});
        }

        onFileDownloadFailed: {
            modalDialogManager.openDialog(messageErrorDialog, {"message": qsTr("Error when trying to create a database backup")});
        }

        onStateChanged: {
            console.log("onStateChanged", remoteFileController.state);
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

            remoteFileController.GetFile("BackupFile", fileName);
        }
    }
}


