import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtgui 1.0
import imtguigql 1.0


/**
    ElementView wrapper around ServerPathPicker.
    Override getHeaders() to pass GQL routing headers (e.g. clientid).
*/

ElementView {
	id: pathPickerElement

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
	property var textInputValidator: null
	property bool showErrorWhenInvalid: false
	property string errorText: ""

	signal pathEdited()
	signal pathsEdited(var paths)
	signal editingFinished()

	controlComp: pickerComp

	function getHeaders(){
		return {}
	}

	Component {
		id: pickerComp

		ServerPathPicker {
			id: picker

			width: pathPickerElement.controlWidth
			height: pathPickerElement.controlHeight
			path: pathPickerElement.path
			pathKind: pathPickerElement.pathKind
			multiSelect: pathPickerElement.multiSelect
			titleText: pathPickerElement.titleText
			browseEnabled: pathPickerElement.browseEnabled
			readOnlyPath: pathPickerElement.readOnlyPath
			placeHolderText: pathPickerElement.placeHolderText
			extensions: pathPickerElement.extensions

			function getHeaders(){
				return pathPickerElement.getHeaders()
			}

			onPathChanged: {
				if (pathPickerElement.path !== picker.path)
					pathPickerElement.path = picker.path
			}

			onPathEdited: {
				pathPickerElement.pathEdited()
			}

			onPathsEdited: {
				pathPickerElement.paths = paths
				pathPickerElement.pathsEdited(paths)
			}

			Connections {
				target: pathPickerElement

				function onPathChanged(){
					if (picker.path !== pathPickerElement.path)
						picker.path = pathPickerElement.path
				}

				function onBrowseEnabledChanged(){
					picker.browseEnabled = pathPickerElement.browseEnabled
				}

				function onPathKindChanged(){
					picker.pathKind = pathPickerElement.pathKind
				}

				function onPlaceHolderTextChanged(){
					picker.placeHolderText = pathPickerElement.placeHolderText
				}
			}
		}
	}
}
