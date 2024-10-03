import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import Qt.labs.platform 1.1
import QtQuick.Dialogs 1.3

ButtonElementView {
    id: root;

    text: qsTr("Browse...");

    property string path;
    property alias fileDialog: fileDialog_;
    signal accepted();

    onClicked: {
        fileDialog_.open();
    }

    FolderDialog {
        id: fileDialog_;
        onAccepted: {
            var pathDir = fileDialog_.folder.toString();
            pathDir = pathDir.replace('file:///', '')
            root.path = pathDir;

            root.accepted();
        }
    }

    bottomComp: Component {
        BaseText {
            text: root.path;
        }
    }
}


