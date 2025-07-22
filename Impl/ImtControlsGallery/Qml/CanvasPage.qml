import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0
import imtcolgui 1.0

SchemeView{
	id: scheme;

	color: Style.baseColor

	Component.onCompleted: {
		//TEST
		//links for test
		let index_link = linkModel.insertNewItem();
		linkModel.setData("ObjectId", "02", index_link);
		index_link = linkModel.insertNewItem();
		linkModel.setData("ObjectId", "03", index_link);
		index_link = linkModel.insertNewItem();
		linkModel.setData("ObjectId", "04", index_link);
		index_link = linkModel.insertNewItem();
		linkModel.setData("ObjectId", "05", index_link);

		let index_link2 = linkModel2.insertNewItem();
		linkModel2.setData("ObjectId", "02", index_link2);
		index_link2 = linkModel2.insertNewItem();
		linkModel2.setData("ObjectId", "03", index_link2);

		let index_link3 = linkModel3.insertNewItem();
		linkModel3.setData("ObjectId", "02", index_link3);
		index_link3 = linkModel3.insertNewItem();
		linkModel3.setData("ObjectId", "03", index_link3);
		//

		// let index = objectModel.insertNewItem();
		// objectModel.setData("Id", "01", index);
		// objectModel.setData("X", 300, index);
		// objectModel.setData("Y", 300, index);
		// objectModel.setData("MainText", "Main text Main text Main text", index);
		// objectModel.setData("SecondText", "Second text", index);
		// objectModel.setExternTreeModel("Links", linkModel, index);

		// index = objectModel.insertNewItem();
		// objectModel.setData("Id", "02", index);
		// objectModel.setData("X", 600, index);
		// objectModel.setData("Y", 600, index);
		// objectModel.setData("MainText", "Main text 2", index);
		// objectModel.setData("SecondText", "Second text 2", index);
		// objectModel.setData("HasError", true, index);

		// index = objectModel.insertNewItem();
		// objectModel.setData("Id", "03", index);
		// objectModel.setData("X", 100, index);
		// objectModel.setData("Y", 100, index);
		// objectModel.setData("MainText", "Main text 3", index);
		// objectModel.setData("SecondText", "Second text 3", index);
		// objectModel.setData("IsComposite", true, index);

		// index = objectModel.insertNewItem();
		// objectModel.setData("Id", "04", index);
		// objectModel.setData("X", 600, index);
		// objectModel.setData("Y", 100, index);
		// objectModel.setData("MainText", "Main text 4", index);
		// objectModel.setData("SecondText", "Second text 4", index);
		// objectModel.setExternTreeModel("Links", linkModel3, index);

		// index = objectModel.insertNewItem();
		// objectModel.setData("Id", "05", index);
		// objectModel.setData("X", 100, index);
		// objectModel.setData("Y", 600, index);
		// objectModel.setData("MainText", "Main text 5", index);
		// objectModel.setData("SecondText", "Second text 5", index);
		// objectModel.setExternTreeModel("Links", linkModel2, index);

		//scheme.requestPaint();
		//TEST
	}

	TreeItemModel {id: linkModel;/*for test*/}
	TreeItemModel {id: linkModel2;/*for test*/}
	TreeItemModel {id: linkModel3;/*for test*/}

	onCopySignal: {
		if(scheme.selectedIndex >=0){
			scheme.copyObjectFunc(scheme.selectedIndex);
		}
	}
	onPasteSignal: {
		if(scheme.selectedIndex >=0){
			scheme.pasteObjectFunc();
		}
	}
	onDeleteSignal: {
		if(scheme.selectedIndex >=0){
			scheme.deleteObjectFunc(scheme.selectedIndex);
		}
	}
}


