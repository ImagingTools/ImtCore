import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: root
	
	typeId: ParamTypeIdsTypeMetaInfo.s_serverConnectionParam
	
	editorModelComp: Component {
		ServerConnectionParam {}
	}
	
	property ServerConnectionParam serverConnectionParam: editorModel

	sourceComp: Component {
		Column {
			id: content
			width: root.width
			spacing: Style.marginXL
			
			GroupHeaderView {
				width: parent.width
				title: root.name
				groupView: generalGroup
			}
			
			GroupElementView {
				id: generalGroup
				width: content.width

				TextInputElementView {
					id: hostInput
					name: qsTr("Server Host")
					text: root.serverConnectionParam ? root.serverConnectionParam.m_host : ""
					onEditingFinished: {
						if (root.serverConnectionParam){
							root.serverConnectionParam.m_host = text
						}
					}
				}

				IntegerInputElementView {
					id: httpPortInput
					name: qsTr("Http Port")
					value: root.serverConnectionParam ? root.serverConnectionParam.m_httpPort : ""
					onEditingFinished: {
						if (root.serverConnectionParam){
							root.serverConnectionParam.m_httpPort = value
						}
					}
				}
				
				IntegerInputElementView {
					id: wsPortInput
					name: qsTr("Web Socket Port")
					text: root.serverConnectionParam ? root.serverConnectionParam.m_wsPort : ""
					onEditingFinished: {
						if (root.serverConnectionParam){
							root.serverConnectionParam.m_wsPort = value
						}
					}
				}
				
				SwitchElementView {
					id: switchElementView
					name: qsTr("Secure Connection")
					checked: root.serverConnectionParam ? root.serverConnectionParam.m_isSecure : false
					
					onCheckedChanged: {
						if (root.serverConnectionParam){
							root.serverConnectionParam.m_isSecure = checked
						}
					}
				}
			}
		}
	}
}
