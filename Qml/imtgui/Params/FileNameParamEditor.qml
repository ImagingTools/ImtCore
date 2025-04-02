import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: filePickerParam
	
	// Path Type
	// 0 - Unknown
	// 1 - File
	// 2 - Directory
	// 3 - Url
	
	property FileNameParam fileNameParam: editorModel
	editorModelComp: Component {
		FileNameParam {}
	}
	
	sourceComp: Component {
		FilePickerElementView {
			id: root;
			width: filePickerParam.width
			
			path: filePickerParam.fileNameParam ? filePickerParam.fileNameParam.m_path : ""
			pathType: filePickerParam.fileNameParam ? filePickerParam.fileNameParam.m_pathType : 1
			name: filePickerParam.name;
			description: filePickerParam.description
			onAccepted: {
				filePickerParam.fileNameParam.m_path = path
			}
		}
	}
}



