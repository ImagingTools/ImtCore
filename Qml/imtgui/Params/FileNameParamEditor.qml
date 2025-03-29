import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: filePickerParam
	
	property FileNameParam fileNameParam: editorModel
	editorModelComp: Component {
		FileNameParam {}
	}
	
	sourceComp: Component {
		FilePickerElementView {
			id: root;
			width: filePickerParam.width
			
			path: filePickerParam.fileNameParam ? filePickerParam.fileNameParam.m_path : ""
			name: filePickerParam.name;
			description: filePickerParam.description
			fileDialog.currentFolder: path
		
			onAccepted: {
				filePickerParam.fileNameParam.m_path = path
			}
		}
	}
}



