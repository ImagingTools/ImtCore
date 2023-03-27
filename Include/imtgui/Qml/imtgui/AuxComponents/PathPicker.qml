import QtQuick 2.0
import Acf 1.0
import QtQuick.Dialogs 1.3


Item {
    id: root;

    width: 270;
    height: 30;

    property string path: model.Value;

    onPathChanged: {
        if (model.Value !== root.path){
            model.Value = root.path;
        }
    }

    Item {
        anchors.fill: parent;

        CustomTextField {
            anchors.left: parent.left;
            anchors.right: button.left;
            anchors.rightMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            height: parent.height;

            text: root.path;

            readOnly: true;

            onTextChanged: {
                ensureVisible(0);
            }
        }

        BaseButton {
            id: button;

            anchors.right: parent.right;
            anchors.verticalCenter: parent.verticalCenter;

            text: "Browse";

            decorator: defaultButtonDecorator;

            onClicked: {
                fileDialog.open();
            }
        }

        Component{
            id: defaultButtonDecorator;

            CommonButtonDecorator{
            }
        }
    }

    FileDialog {
        id: fileDialog;

        title: qsTr("Select folder");
        selectExisting: false;
        folder: shortcuts.home;

        selectFolder: true;

        onAccepted: {
            var pathDir = fileDialog.fileUrl.toString();

            console.log("fileUrl", fileDialog.fileUrl);

            pathDir = pathDir.replace('file:///', '')

            root.path = pathDir;
        }
    }
}
