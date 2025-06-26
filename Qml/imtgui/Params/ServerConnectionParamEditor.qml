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

				onParamsChanged: {
					if (root.serverConnectionParam){
						root.serverConnectionParam.m_host = host
						root.serverConnectionParam.m_httpPort = httpPort
						root.serverConnectionParam.m_wsPort = wsPort
						root.serverConnectionParam.m_isSecure = isSecure
					}
				}
			}
		}
	}
}
