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

			ServerConnectionParamElementView {
				id: generalGroup
				width: parent.width
				host: root.serverConnectionParam ? root.serverConnectionParam.m_host : ""
				httpPort: root.serverConnectionParam ? root.serverConnectionParam.m_httpPort : -1
				wsPort: root.serverConnectionParam ? root.serverConnectionParam.m_wsPort : -1
				isSecure: root.serverConnectionParam ? root.serverConnectionParam.m_isSecure : false

				onHostChanged: {
					if (root.serverConnectionParam){
						root.serverConnectionParam.m_host = host
					}
				}

				onHttpPortChanged: {
					if (root.serverConnectionParam){
						root.serverConnectionParam.m_httpPort = httpPort
					}
				}

				onWsPortChanged: {
					if (root.serverConnectionParam){
						root.serverConnectionParam.m_wsPort = wsPort
					}
				}

				onIsSecureChanged: {
					if (root.serverConnectionParam){
						root.serverConnectionParam.m_isSecure = isSecure
					}
				}
			}
		}
	}
}
