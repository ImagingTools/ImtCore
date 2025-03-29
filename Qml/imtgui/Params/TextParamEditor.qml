import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: textParamEditor
	
	typeId: ParamTypeIdsTypeMetaInfo.s_textParam
	
	editorModelComp: Component {
		TextParam {}
	}
	
	property TextParam textParam: editorModel

	sourceComp: Component {
		TextInputElementView {
			width: textParamEditor.width
			name: textParamEditor.name
			description: textParamEditor.description
			controlWidth: 300
			text: textParamEditor.textParam ? textParamEditor.textParam.m_text : ""
			readOnly: textParamEditor.readOnly
			onEditingFinished: {
				if (textParamEditor.textParam){
					let oldText = textParamEditor.textParam.m_text
					textParamEditor.textParam.m_text = text
				}
			}
		}
	}
}


