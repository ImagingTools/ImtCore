import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0


/**
    Path input with Browse button. Opens FileSystemBrowserDialog.
    Override getHeaders() to pass GQL routing headers (e.g. clientid).
*/

Item {
	id: serverPathPicker

	width: Style.sizeHintM
	height: Style.controlHeightM

	property string path: ""
	property var paths: []
	// Bit-mask of Enums.pathKindFile / Enums.pathKindDir (see Enums.qml).
	property int pathKind: Enums.pathKindDir
	property bool multiSelect: false
	property string titleText: ""
	property bool browseEnabled: true
	property bool readOnlyPath: false
	property string placeHolderText: ""
	// Optional file-extension whitelist (without dots, e.g. ["exe", "dll"]).
	// Empty = all files shown. Use "*" to also allow extensionless names
	// (Linux binaries). Files only; folders are always navigable.
	property var extensions: []

	signal pathEdited()
	signal pathsEdited(var paths)

	function getHeaders(){
		return {}
	}

	Component {
		id: browserDialogComp

		FileSystemBrowserDialog {
			onPathSelected: {
				serverPathPicker.path = selectedPath
				serverPathPicker.pathEdited()
			}

			onPathsSelected: {
				serverPathPicker.paths = paths
				serverPathPicker.pathsEdited(paths)
			}
		}
	}

	CustomTextField {
		id: pathField

		anchors.left: parent.left
		anchors.right: browseButton.left
		anchors.rightMargin: Style.marginM
		anchors.verticalCenter: parent.verticalCenter
		height: parent.height
		text: serverPathPicker.path
		readOnly: serverPathPicker.readOnlyPath
		placeHolderText: serverPathPicker.placeHolderText

		onEditingFinished: {
			if (serverPathPicker.readOnlyPath)
				return
			if (pathField.text === serverPathPicker.path)
				return
			serverPathPicker.path = pathField.text
			serverPathPicker.pathEdited()
		}
	}

	Button {
		id: browseButton

		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		widthFromDecorator: true
		text: qsTr("Browse...")
		enabled: serverPathPicker.browseEnabled

		onClicked: {
			if (!serverPathPicker.browseEnabled)
				return

			// ListModel in ModalDialogManager does not reliably store nested JS objects.
			// Pass routing headers as plain strings only.
			var headers = serverPathPicker.getHeaders()
			var params = {}
			params.startPath = serverPathPicker.path
			params.pathKind = serverPathPicker.pathKind
			params.multiSelect = serverPathPicker.multiSelect
			params.titleText = serverPathPicker.titleText
			params.clientIdHeader = headers["clientid"] ? ("" + headers["clientid"]) : ""
			params.serviceIdHeader = headers["serviceid"] ? ("" + headers["serviceid"]) : ""
			// ModalDialogManager's ListModel does not reliably carry arrays, so
			// pass the extension whitelist as a single ';'-separated string.
			params.extensionFilter = (serverPathPicker.extensions && serverPathPicker.extensions.length > 0)
				? serverPathPicker.extensions.join(";") : ""

			ModalDialogManager.openDialog(browserDialogComp, params)
		}
	}

	onPathChanged: {
		if (pathField.text !== serverPathPicker.path)
			pathField.text = serverPathPicker.path
	}
}
