import QtQuick 2.0
import Acf 1.0
import QtQuick.Dialogs 1.3
import imtcontrols 1.0
import Qt.labs.platform 1.1

Item {
    id: root;

    width: 400;
    height: 30;

    property string path: model.Path;

    onPathChanged: {
        if (model.Path !== root.path){
            model.Path = root.path;
        }

        tfc.ensureVisible(0)
    }

    Component.onCompleted: {
        // fileDialog.currentFolder = model.Path;
    }

    Item {
        anchors.fill: parent;

        CustomTextField {
            id: tfc;

            anchors.left: parent.left;
            anchors.right: button.left;
            anchors.rightMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            height: parent.height;

            text: root.path;

            readOnly: true;
        }

        Button {
            id: button;

            anchors.right: parent.right;
            anchors.verticalCenter: parent.verticalCenter;

            text: "Browse...";

            onClicked: {
                // fileDialog.open();
            }
        }
    }

//    FolderDialog {
//        id: fileDialog;

//        onAccepted: {
//            var pathDir = fileDialog.folder.toString();
//            console.log("fileUrl", fileDialog.folder);
//            pathDir = pathDir.replace('file:///', '')
//            root.path = pathDir;
//        }
//    }
}
