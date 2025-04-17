import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import Qt.labs.platform 1.1
import QtQuick.Dialogs 1.3

ButtonElementView {
	id: root
	
	text: qsTr("Browse...")
	
	property int pathType: 1
	property string path
	property alias fileDialog: fileDialog_
	property alias folderDialog: folderDialog_
	
	signal accepted()
	
	onClicked: {
		if (pathType == 1){
			fileDialog_.currentFile = 'file:///' + path
			fileDialog_.open()
		}
		else if (pathType == 2){
			folderDialog_.currentFolder = path
			folderDialog_.open()
		}
	}
	
	FileDialog {
		id: fileDialog_
		title: qsTr("Select a file")
		fileMode: FileDialog.OpenFile
		
		onAccepted: {
			let filePath = fileDialog_.file.toString()
			filePath = filePath.replace('file:///', '')
			root.path = filePath
			
			root.accepted()
		}
	}
	
	FolderDialog {
		id: folderDialog_
		onAccepted: {
			var pathDir = folderDialog_.folder.toString()
			pathDir = pathDir.replace('file:///', '')
			root.path = pathDir
			
			root.accepted()
		}
	}
	
	bottomComp: Component {
		BaseText {
			text: root.path == "" ? qsTr("Nothing is selected") : root.path
		}
	}
}


