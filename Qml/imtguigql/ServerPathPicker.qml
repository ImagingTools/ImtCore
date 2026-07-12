import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0


/**
    Path input with a Browse button opening the server-side file system browser.

    Usage example:

    ServerPathPicker {
        onPathChanged: {
            console.log("Selected path:", path);
        }
    }
*/

Item {
	id: root;

	width: Style.sizeHintM;
	height: Style.controlHeightM;

	property string path: "";
	property bool showFiles: false;

	Component {
		id: browserDialogComp;

		FileSystemBrowserDialog {
			startPath: root.path;
			showFiles: root.showFiles;

			onPathSelected: {
				root.path = selectedPath;
			}
		}
	}

	CustomTextField {
		id: pathField;

		anchors.left: parent.left;
		anchors.right: browseButton.left;
		anchors.rightMargin: Style.marginM;
		anchors.verticalCenter: parent.verticalCenter;

		height: parent.height;

		text: root.path;

		readOnly: true;
	}

	Button {
		id: browseButton;

		anchors.right: parent.right;
		anchors.verticalCenter: parent.verticalCenter;

		widthFromDecorator: true;

		text: qsTr("Browse...");

		onClicked: {
			ModalDialogManager.openDialog(browserDialogComp, {});
		}
	}
}
