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
	
	property string json: "{\"type\": \"data\",\"id\": \"70e4d6b5-7afe-4490-9b80-09636fc825e2\",\"payload\": {\"data\": {\"OnAccountsCollectionChanged\": {\"itemIds\":[\"dce1f9da-8a1b-40c7-b903-49076ab7e550\",\"0722c437-dbaf-4a09-ac64-411ca5bcb61c\"],\"operationContext\":{\"ownerId\":\"su\",\"ownerName\":\"superuser\"},\"typeOperation\":\"removed\"}}}}"
	
	TreeItemModel {
		id: treeItemModel
	}
	
	TreeItemModel {
		id: treeItemModel2
	}
	
	Row {
		height: 30;

		Button {
			width: 100;
			height: 30;
			text: "Test";
			onClicked: {
				treeItemModel.createFromJson(testPage.json)
				console.log("treeItemModel", treeItemModel.toJson())
				treeItemModel2.copy(treeItemModel)
				console.log("treeItemModel2", treeItemModel2.toJson())
			}
		}
	}
}
