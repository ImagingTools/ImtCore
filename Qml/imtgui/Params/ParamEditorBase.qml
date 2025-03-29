import QtQuick 2.15

Item {
	id: paramEditorBase
	
	width: loader.width
	height: loader.item ? loader.item.height : 0

	property string paramId
	property string typeId
	property string name
	property string description
	property bool readOnly: false
	property var editorModel: null
	property alias editorModelComp: paramEditorBase.paramController.paramFactoryComp
	property alias sourceComp: loader.sourceComponent
	property alias sourceItem: loader.item
	property ParamController paramController: ParamController {}
	
	signal editorModelDataChanged(string paramId, string key);

	Connections {
		target: paramEditorBase.paramController
		
		function onParamModelCreated(){
			paramEditorBase.editorModel = paramEditorBase.paramController.paramModel
		}
	}
	
	Connections {
		target: paramEditorBase.editorModel
		function onModelChanged(changeSet){
			let propName = "";
			if (changeSet.length == 1){
				propName = changeSet[0].name
			}
			
			paramEditorBase.editorModelDataChanged(paramEditorBase.paramId, propName)
		}
	}
	
	Loader {
		id: loader
	}
}
