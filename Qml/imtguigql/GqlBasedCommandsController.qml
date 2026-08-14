import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtbaseCommandsSdl 1.0

CommandsController {
	id: root;
	
	property string context: ""
	function getCommands(){
		commandsRequest.send()
	}
	
	function getHeaders(){
		return {};
	}
	
	property GqlSdlRequestSender commandsRequest: GqlSdlRequestSender {
		context: root.context
		gqlCommandId: ImtbaseCommandsSdlCommandIds.s_getCommands;
		inputObjectComp: Component {
			GetCommandsInput {
				m_typeId: root.typeId;
			}
		}
		
		sdlObjectComp: Component {
			GuiElementContainer {
				onFinished: {
					root.commands = this
					root.commandsReceived(root.typeId, this);
				}
			}
		}
		
		function getHeaders(){
			return root.getHeaders();
		}
	}
}

