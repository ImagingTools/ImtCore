import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import Qt.labs.platform 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0
import imtcontrols 1.0

ParamEditorBase {
	id: dbEditor
	
	property DatabaseAccessSettings databaseParams: editorModel
	editorModelComp: Component {
		DatabaseAccessSettings {}
	}
	
	RemoteFileController {
		id: remoteFileController;
		prefix: "/files/";
		
		onFileUploaded: {
			ModalDialogManager.showInfoDialog(qsTr("Database restore was successful"))
		}
		
		onFileUploadFailed: {
			ModalDialogManager.showErrorDialog(qsTr("Error when trying to restore the database"))
		}
		
		onFileDownloadFailed: {
			ModalDialogManager.showErrorDialog(qsTr("Error when trying to create a database backup"))
		}
		
		onStateChanged: {
			if (remoteFileController.state === "Loading"){
				EventManager.sendEvent("StartLoading");
			}
			else{
				EventManager.sendEvent("StopLoading");
			}
		}
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
	
	sourceComp: Component {
		Column {
			id: content
			width: dbEditor.width
			spacing: Style.marginXL
			
			GroupHeaderView {
				width: parent.width;
				title: dbEditor.name
				groupView: generalGroup;
			}
			
			GroupElementView {
				id: generalGroup
				width: content.width
				
				property alias databaseNameInput: databaseNameInput_
				property alias hostInput: hostInput_
				property alias passwordInput: passwordInput_
				property alias portInput: portInput_
				property alias usernameInput: usernameInput_
				
				TextInputElementView {
					id: databaseNameInput_
					name: qsTr("Database name")
					text: dbEditor.databaseParams ? dbEditor.databaseParams.m_dbName : ""
					onEditingFinished: {
						dbEditor.databaseParams.m_dbName = text
					}
				}
				
				TextInputElementView {
					id: hostInput_
					name: qsTr("Host")
					readOnly: true
					text: dbEditor.databaseParams ? dbEditor.databaseParams.m_host : ""
					onEditingFinished: {
						dbEditor.databaseParams.m_host = text
					}
				}
				
				TextInputElementView {
					id: passwordInput_
					name: qsTr("Password")
					echoMode: TextInput.Password
					text: dbEditor.databaseParams ? dbEditor.databaseParams.m_password : ""
					onEditingFinished: {
						dbEditor.databaseParams.m_password = text
					}
				}
				
				TextInputElementView {
					id: portInput_
					name: qsTr("Port")
					text: dbEditor.databaseParams ? dbEditor.databaseParams.m_port : ""
					onEditingFinished: {
						dbEditor.databaseParams.m_port = text
					}
				}
				
				TextInputElementView {
					id: usernameInput_
					name: qsTr("Username")
					readOnly: true
					text: dbEditor.databaseParams ? dbEditor.databaseParams.m_username : ""
					onEditingFinished: {
						dbEditor.databaseParams.m_username = text
					}
				}
				
				ButtonElementView {
					id: buttonElementView;
					width: parent.width;
					name: qsTr("Test database connection");
					text: qsTr("Test");
					onClicked: {
						let env = "PGPASSWORD=" + passwordInput_.text;
						process.setEnviroment([env])
						process.start("psql", ["-U", "postgres", "-d", databaseNameInput_.text, "-c", "SELECT 1"]);
					}
					
					Process {
						id: process;
						onFinished: {
							buttonElementView.bottomComp = exitCode != 0 ? errorComp : connectionComp;
						}
					}
					
					Component {
						id: errorComp;
						BaseText {
							text: qsTr("There is no connection to the database");
							color: Style.errorTextColor;
						}
					}
					
					Component {
						id: connectionComp;
						BaseText {
							text: qsTr("Test connection successfully");
							color: Style.greenColor;
						}
					}
				}
			} // GroupElementView
			
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
			} // GroupElementView
		}
	}
}


