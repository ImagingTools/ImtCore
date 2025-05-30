import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: enableableParamEditor
	
	typeId: ParamTypeIdsTypeMetaInfo.s_enableableParam
	
	editorModelComp: Component {
		EnableableParam {}
	}
	
	property EnableableParam enableableParam: editorModel

	sourceComp: Component {
		SwitchElementView {
			width: enableableParamEditor.width
			name: enableableParamEditor.name
			description: enableableParamEditor.description
			controlWidth: Style.sizeHintXS
			checked: enableableParamEditor.enableableParam ? enableableParamEditor.enableableParam.m_value : false
			readOnly: enableableParamEditor.readOnly
			onCheckedChanged: {
				if (enableableParamEditor.enableableParam){
					enableableParamEditor.enableableParam.m_value = checked
				}
			}
		}
	}
}


