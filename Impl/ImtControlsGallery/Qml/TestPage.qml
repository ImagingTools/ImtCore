import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0

Rectangle {
	id: testPage;

	anchors.fill: parent;
	clip: true;
	color: Style.baseColor
	
	Row {
		id: rowContent
		height: 40
		
		Button {
			width: 100
			height: 40
			text: "5 elem"
			onClicked: {
				testPage.updateTreeModel(5)
			}
		}
		
		Button {
			width: 100
			height: 40
			text: "50 elem"
			onClicked: {
				testPage.updateTreeModel(50)
			}
		}
		
		Button {
			width: 100
			height: 40
			text: "100 elem"
			onClicked: {
				testPage.updateTreeModel(100)
			}
		}
	}
	
	function updateTreeModel(count){
		treeModel.clear()
		
		for (let i = 1; i <= count; i++){
			let index = treeModel.insertNewItem()
			treeModel.setData("Test1", "Test" + i, index)
			treeModel.setData("Test2", "Test" + (i + 1), index)
			if (i % 2 == 0){
				let childModel = treeModel.addTreeModel("ChildModel", index)
				if (childModel){
					for (let j = 0; j < 5; j++){
						let index2 = childModel.insertNewItem()
						childModel.setData("Test1", "Test" + j, index2)
						childModel.setData("Test2", "Test" + (j + 1), index2)
					}
				}
			}
			else{
				treeModel.setData("ChildModel", 0, index)
			}
		}
		
		treeView.columnModel = headersModel
		treeView.rowModel = treeModel
	}
	
	function updateHeaders(){
		headersModel.clear()
		
		let index = headersModel.insertNewItem();
		headersModel.setData("id", "Test1", index);
		headersModel.setData("name", "Test1", index);
		
		index = headersModel.insertNewItem();
		headersModel.setData("id", "Test2", index);
		headersModel.setData("name", "Test2", index);
		
		treeView.columnModel = headersModel
	}
	
	TreeItemModel {
		id: treeModel
	}
	
	TreeItemModel {
		id: headersModel
		Component.onCompleted: {
			testPage.updateHeaders()
		}
	}
	
	BasicTreeView {
		id: treeView
		anchors.top: rowContent.bottom
		width: parent.width
		height: 500
	}
}
