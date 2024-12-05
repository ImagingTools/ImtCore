import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtappApplicationSdl 1.0
import imtguigql 1.0

Rectangle {
	id: testPage;

	anchors.fill: parent;
	clip: true;

	property int columnCount: 20
	property int rowCount: 200

	Component.onCompleted: {

	}

	// TestInfo1 {
	// 	id: test1;
	// 	m_test: "fdsfsdf";
	// }

	// BaseModel {
	// 	id: baseModel;
	// 	Component.onCompleted: {
	// 		addElement(test1)
	// 		addElement(test1)
	// 		addElement(test1)
	// 	}
	// }

	// property GqlSdlRequestSender request : GqlSdlRequestSender {
	// 	gqlCommandId: ImtappApplicationSdlCommandIds.s_getApplicationInfo;
	// 	inputObjectComp: Component {
	// 		GetApplicationInfoInput {
	// 			m_productId: "Test";
	// 			m_testInfos: baseModel;
	// 		}
	// 	}

	// 	sdlObjectComp: Component { ApplicationInfo {
	// 		} }
	// }

	// Button {
	// 	width: 100;
	// 	height: 30;
	// 	text: "test"

	// 	onClicked: {
	// 		testPage.request.send();
	// 	}
	// }

	Table {
		id: table;
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;
		width: parent.width;
		hasFilter: true;
		hasSort: true;
		canMoveColumns: true;

		TreeItemModel {
			id: elementsModel;

			Component.onCompleted: {

				for (let i = 0; i < testPage.rowCount; i++){
					elementsModel.insertNewItem();

					for (let j = 1; j <= testPage.columnCount; j++){
						elementsModel.setData("Test" + j, "Test Data" + j, i);
					}
				}

				table.headers = headersModel;
				table.elements = elementsModel;
			}
		}

		onHeaderRightMouseClicked: {
			ModalDialogManager.openDialog(tableHeaderParamComp, {});
		}

		onHeadersChanged: {
			resetViewParams()
		}

		Component {
			id: tableHeaderParamComp;

			TableHeaderParamComp{
				tableItem: table;
				onFinished: {
				}
			}
		}


		TreeItemModel {
			id: headersModel;

			Component.onCompleted: {
				for (let i = 1; i <= testPage.columnCount; i++){
					let index = headersModel.insertNewItem();
					headersModel.setData("Id", "Test" + i, index);
					headersModel.setData("Name", "Test" + i, index);
				}
			}
		}
	}
}
