import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Rectangle {
	id: testPage;

	anchors.fill: parent;
	clip: true;
	color: Style.baseColor;

	function fillModel(rowCount) {
		let data = {
			headers: [],
			rows: []
		};

		for (let column = 0; column < 25; column++){
			data.headers.push({
				id: "c" + column,
				name: qsTr("Колонка ") + (column + 1)
			});
		}

		for (let row = 0; row < rowCount; row++){
			let item = {};
			for (let column = 0; column < 25; column++)
				item["c" + column] = (row + 1) + "." + (column + 1);
			data.rows.push(item);
		}

		tableDocument.fromJSON(JSON.stringify(data));
		canvasTable.headers = tableDocument.m_headers;
		canvasTable.elements = tableDocument.m_rows;
		canvasTable.requestPaint();
	}

	Row {
		id: buttonRow;

		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.topMargin: Style.marginM;
		anchors.leftMargin: Style.marginM;

		spacing: Style.marginS;

		Button {
			objectName: "FillCanvasTableButton25";
			text: "25";
			onClicked: {
				console.time("[DEBUG] fillModel");
				testPage.fillModel(25);
				console.timeEnd("[DEBUG] fillModel");
			}
		}

		Button {
			objectName: "FillCanvasTableButton50";
			text: "50";
			onClicked: {
				console.time("[DEBUG] fillModel");
				testPage.fillModel(50);
				console.timeEnd("[DEBUG] fillModel");
			}
		}

		Button {
			objectName: "FillCanvasTableButton100";
			text: "100";
			onClicked: {
				console.time("[DEBUG] fillModel");
				testPage.fillModel(100);
				console.timeEnd("[DEBUG] fillModel");
			}
		}

		Button {
			objectName: "FillCanvasTableButton250";
			text: "250";
			onClicked: {
				console.time("[DEBUG] fillModel");
				testPage.fillModel(250);
				console.timeEnd("[DEBUG] fillModel");
			}
		}

		Button {
			objectName: "FillCanvasTableButton500";
			text: "500";
			onClicked: {
				console.time("[DEBUG] fillModel");
				testPage.fillModel(500);
				console.timeEnd("[DEBUG] fillModel");
			}
		}
	}

	CanvasTable {
		id: canvasTable;

		anchors.top: buttonRow.bottom;
		anchors.topMargin: Style.marginM;
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginM;
		anchors.right: parent.right;
		anchors.rightMargin: Style.marginM;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.marginM;

		isFlickable: true;
		defaultColumnWidth: Style.sizeHintBXS;
	}

	CanvasTableSdlDocument {
		id: tableDocument;
	}
}
