import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: integerParamEditor
	
	typeId: ParamTypeIdsTypeMetaInfo.s_integerParam
	
	editorModelComp: Component {
		IntegerParam {}
	}
	
	property IntegerParam valueParam: editorModel

	sourceComp: Component {
		IntegerInputElementView {
			width: integerParamEditor.width
			name: integerParamEditor.name
			description: integerParamEditor.description
			controlWidth: 300
			text: integerParamEditor.valueParam ? integerParamEditor.valueParam.m_value : 0
			readOnly: integerParamEditor.readOnly
			onEditingFinished: {
				console.log("onEditingFinished", value, integerParamEditor.valueParam)
				if (integerParamEditor.valueParam){
					integerParamEditor.valueParam.m_value = value
					console.log("ok", value)
					
				}
			}
		}
	}
}
