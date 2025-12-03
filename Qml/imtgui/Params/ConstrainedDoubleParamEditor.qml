import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: doubleParamEditor
	
	typeId: ParamTypeIdsTypeMetaInfo.s_constrainedDoubleParam
	
	editorModelComp: Component {
		ConstrainedDoubleParam {}
	}
	
	property ConstrainedDoubleParam valueParam: editorModel
	property real bottomValue: valueParam.m_minValue !== undefined ? valueParam.m_minValue.m_value : Number.NEGATIVE_INFINITY
	property real topValue: valueParam.m_maxValue !== undefined ? valueParam.m_maxValue.m_value : Number.POSITIVE_INFINITY

	sourceComp: Component {
		DoubleInputElementView {
			width: doubleParamEditor.width
			name: doubleParamEditor.name
			description: doubleParamEditor.description
			controlWidth: Style.sizeHintXS
			text: doubleParamEditor.valueParam ? doubleParamEditor.valueParam.m_value : 0
			readOnly: doubleParamEditor.readOnly

			topValue: doubleParamEditor.topValue
			bottomValue: doubleParamEditor.bottomValue

			onEditingFinished: {
				if (doubleParamEditor.valueParam){
					doubleParamEditor.valueParam.m_value = value
				}
			}
		}
	}
}
