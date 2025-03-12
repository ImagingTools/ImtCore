import QtQuick 2.12
import Acf 1.0;
import imtcontrols 1.0

Item{
	id: inputView;

	property TreeItemModel model: TreeItemModel{};
	property TreeItemModel dataModel: TreeItemModel{};
	property int spacing: 20;
	property alias contentHeight: list.contentHeight;
	property alias contentWidth: list.contentWidth;
	property bool fitToWidth: false;
	property int orientation: Qt.Vertical;
	property alias scrollbar: scroll;

	signal modelDataChanged(string role, int index);


	function setChildParam(param, val, index_){
		list.children[0].children[index_].item[param] = val;
	}

	onModelDataChanged: {
		// console.log("Role:: ", role, "Index:: ", index);
	}

	onModelChanged: {
		// console.log("INPUT_MODEL:: ", model.toJson())
	}

	ListView{
		id: list;

		anchors.fill: parent;

		clip: true;
		spacing: inputView.spacing;
		boundsBehavior: Flickable.StopAtBounds;
		orientation: inputView.orientation == Qt.Vertical ? ListView.Vertical : ListView.Horizontal
		cacheBuffer: 5000;
		model: inputView.model;
		delegate:
			Loader{
			id: loader;

			source: model.Source !==undefined ? model.Source : "";
			function setValue(value_){
				item.value = value_
			}
			onLoaded: {
				if(inputView.fitToWidth){
					item.width = inputView.width;
				}
				if(item.dataModel !==undefined){
					item.dataModel = inputView.dataModel;
				}
				if(item.index !==undefined){
					item.index = model.index;
				}
				if(item.modelDataChanged !==undefined){
					item.modelDataChanged.connect(inputView.modelDataChanged)
				}

				let keys = [];
				keys = inputView.model.getKeys(model.index);
				for(let i = 0; i < keys.length; i++){
					//console.log("KEY::", keys[i], item[keys[i]])
					if(keys[i] == "value"){
						loader.setValue(inputView.model.getData(keys[i], model.index));
					}
					else {
						if(item[keys[i]] !==undefined){
							if(inputView.model.isTreeModel(keys[i], model.index)){
								//console.log("IS_MODEL:: ", keys[i])
								item[keys[i]] = inputView.model.getTreeItemModel(keys[i], model.index);
							}
							else {
								item[keys[i]] = inputView.model.getData(keys[i], model.index);
							}
						}
					}
				}

			}
		}
	}//list

	CustomScrollbar{
		id: scroll;

		anchors.right: list.right;
		anchors.bottom: list.bottom;

		targetItem: list;
		secondSize: 16;
		//indicatorColor: Style.grayColor;
		z: 20;
	}
}

