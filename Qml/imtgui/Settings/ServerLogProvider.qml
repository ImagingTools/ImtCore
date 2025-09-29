import QtQuick 2.12
import QtQuick.Dialogs 1.3
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import Qt.labs.platform 1.1

ParamEditorBase {
	id: serverLogProvider
	
	typeId: "ServerLog"

	sourceComp: Component {
		ButtonElementView {
			id: root;
			width: serverLogProvider.width
			name: qsTr("Server Log");
			text: qsTr("Download");
			
			onClicked: {
				fileDialogSave.open();
			}
			
			RemoteFileController {
				id: remoteFileController;
				prefix: "/files/";
			}
			
			FileDialog {
				id: fileDialogSave;
				title: qsTr("Save file");
				fileMode: FileDialog.SaveFile;
				currentFile: "file:///Log.txt";
				nameFilters: ["Text files (*.txt)", "All files (*)"];
				onAccepted: {
					var pathDir = fileDialogSave.folder.toString();
					remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
					var fileName = fileDialogSave.file.toString().replace(pathDir + "/", '');
					var id = "GetServerLog";
					if (fileName == "") fileName = "ServerLog.txt";
					remoteFileController.getFile(id, fileName);
				}
			}
		}
		
	}
}


