import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: selectionParamEditor
	
	property SelectionParam selectionParam: editorModel
	editorModelComp: Component {
		SelectionParam {}
	}
	
	sourceComp: Component {
		ComboBoxElementView {
			width: selectionParamEditor.width
			name: selectionParamEditor.name
			description: selectionParamEditor.description
			model: selectionParamEditor.selectionParam ? selectionParamEditor.selectionParam.m_constraints.m_options : 0
			currentIndex: selectionParamEditor.selectionParam ? selectionParamEditor.selectionParam.m_selectedIndex : -1
			controlWidth: 300
			nameId: "m_name"
		
			onCurrentIndexChanged: {
				if (selectionParamEditor.selectionParam){
					// let oldIndex = selectionParamEditor.selectionParam.m_selectedIndex
					selectionParamEditor.selectionParam.m_selectedIndex = currentIndex
					
					// let oldOptionId = selectionParamEditor.selectionParam.m_constraints.m_options.get(oldIndex).item.m_id
					// let newOptionId = selectionParamEditor.selectionParam.m_constraints.m_options.get(currentIndex).item.m_id
					// if (oldOptionId != newOptionId){
					// 	selectionParamEditor.editorModelDataChanged(selectionParamEditor.paramId, "optionId", oldOptionId, newOptionId)
					// }
				}
			}
		}
	}
}


