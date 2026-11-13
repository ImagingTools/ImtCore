import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0
import imtcontrols 1.0

ParamEditorBase {
	id: dbEditor
	
	property DatabaseAccessSettings databaseParams: editorModel
	property Component backupComp: null
	editorModelComp: Component {
		DatabaseAccessSettings {}
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
			
			Loader {
				width: content.width
				sourceComponent: dbEditor.backupComp
			}
		}
	}
}


