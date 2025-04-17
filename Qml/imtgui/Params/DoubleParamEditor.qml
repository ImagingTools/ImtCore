import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: doubleParamEditor
	
	typeId: ParamTypeIdsTypeMetaInfo.s_doubleParam
	
	editorModelComp: Component {
		DoubleParam {}
	}
	
	property DoubleParam valueParam: editorModel

	sourceComp: Component {
		DoubleInputElementView {
			width: doubleParamEditor.width
			name: doubleParamEditor.name
			description: doubleParamEditor.description
			controlWidth: 300
			text: doubleParamEditor.valueParam ? doubleParamEditor.valueParam.m_value : 0
			readOnly: doubleParamEditor.readOnly
			onEditingFinished: {
				if (doubleParamEditor.valueParam){
					doubleParamEditor.valueParam.m_value = value
				}
			}
		}
	}
}
