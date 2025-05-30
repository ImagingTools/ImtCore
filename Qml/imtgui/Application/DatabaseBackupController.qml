import QtQuick 2.0
import Qt.labs.platform 1.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: root;

    width: 300;
    height: content.height;

    Column {
        id: content;
        width: parent.width;
        spacing: Style.marginM;

        GroupHeaderView {
            id: headerView;
            width: parent.width;
            title: qsTr("Backup Information");
            groupView: group;
        }

        GroupElementView {
            id: group;
            width: parent.width;

            ButtonElementView {
                id: backupButton;
                width: parent.width;
                name: qsTr("Backup data");
                description: qsTr("Before performing the operation, check the connection to the server");
                text: qsTr("Backup");
                onClicked: {
                    fileDialogSave.open();
                }
            }

            ButtonElementView {
                width: parent.width;
                name: qsTr("Restore data from backup");
                text: qsTr("Restore");
                description: backupButton.description;
                onClicked: {
                    fileDialog.open();
                }
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

			remoteFileController.sendFile(filePath);
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

            remoteFileController.getFile("GetBackupFile", fileName);
        }
    }
}


