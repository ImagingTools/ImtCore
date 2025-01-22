import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Row {
	id: paginationContainer;

	height: visible ? 20 : 0;
	spacing: 15;

	property int pagesSize: 1;
	property int currentValue: currentIndex + 1;
	property int currentIndex: 0;

	// The number of elements on the page
	property int countElements: 25;

	property int countAllElements: 0;

	property alias pageCount: listModel.count;

	Component.onCompleted: {
		paginationContainer.refreshBtn();
	}

	onPagesSizeChanged: {
		if (paginationContainer.currentIndex > paginationContainer.pagesSize - 1){
			paginationContainer.currentIndex = 0;
		}

		paginationContainer.refreshBtn();
	}

	onCurrentIndexChanged: {
		if (paginationContainer.pagesSize >= props.maxElementCount){
			paginationContainer.refreshBtn();
		}
	}

	Component {
		id: buttonDecorator;

		ButtonDecorator {
			color: "transparent";
			border.width: 0;
		}
	}

	ListModel {
		id: listModel;
	}

	QtObject {
		id: props;

		property int maxElementCount: 6;
	}

	function refreshBtn(){
		listModel.clear();
		if (paginationContainer.pagesSize < props.maxElementCount){
			for (var i = 0; i < paginationContainer.pagesSize; i++){
				listModel.append({number: i+1, selected: paginationContainer.currentIndex + 1 === i + 1});
			}
		}
		else {
			for (let i = 1; i <= 2; i++){
				listModel.append({number: i, selected: paginationContainer.currentIndex + 1 === i})
			}

			listModel.append({number: paginationContainer.currentIndex + 1 - 2 > 1 + 3 ? -1 : 1 + 2, selected: paginationContainer.currentIndex + 1 === 3});
			for (var k = Math.max(1 + 3, paginationContainer.currentIndex + 1 - 2); k <= Math.min(paginationContainer.pagesSize - 3, paginationContainer.currentIndex + 1 + 2); k++){
				listModel.append({number: k, selected: paginationContainer.currentIndex + 1 === k});
			}

			listModel.append({number: paginationContainer.currentIndex + 1 + 2 < paginationContainer.pagesSize - 3 ? -1 : paginationContainer.pagesSize - 2, selected: paginationContainer.currentIndex + 1 === paginationContainer.pagesSize - 2});

			for (let i = paginationContainer.pagesSize - 1; i <= paginationContainer.pagesSize; i++){
				listModel.append({number: i, selected: paginationContainer.currentIndex + 1 === i})
			}
		}
		repeaterPagination.model = listModel;
	}

	BaseText {
		anchors.verticalCenter: parent.verticalCenter;

		text: "(" + (paginationContainer.currentIndex * paginationContainer.countElements + 1) + "-" +
			  ((paginationContainer.pagesSize - 1 != paginationContainer.currentIndex) ?
				   paginationContainer.currentValue * paginationContainer.countElements :
				   paginationContainer.countAllElements) + "/" + paginationContainer.countAllElements + ")"
	}

	Button {
		id: buttonDecr;

		anchors.verticalCenter: parent.verticalCenter;

		height: 20;
		width: 24;

		enabled: listModel.count > 1 && paginationContainer.currentIndex != 0;

		iconSource: buttonDecr.enabled ? "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal):
										 "../../../" + Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled);

		decorator: buttonDecorator;

		onClicked: {
			if (paginationContainer.currentIndex >= 1){
				paginationContainer.currentIndex--;
			}
		}
	}

	Row{
		anchors.verticalCenter: parent.verticalCenter;

		spacing: Style.size_smallMargin;

		Repeater {
			id: repeaterPagination;

			delegate: Component {Button {
					id: buttonDelegate;

					width: Math.max(24, helperText.width + 8);
					height: 20;

					decorator: buttonDecorator;

					enabled: model.number !== -1 && paginationContainer.currentIndex !== model.number - 1;
					text: model.number === -1 ? "..." : model.number;

					onClicked: {
						paginationContainer.currentIndex = model.number - 1;
					}

					Rectangle {
						anchors.top: buttonDelegate.bottom;
						width: parent.width;
						height: 2;

						color: Style.tabSelectedColor;
						visible: (model.index == undefined ||  model.selected == undefined) ? false : paginationContainer.pagesSize < props.maxElementCount ? model.index === paginationContainer.currentIndex : model.selected;
					}

					BaseText {
						id: helperText;
						text: buttonDelegate.text;
						visible: false;
					}
				}
			}
		}
	}

	Button {
		id: buttonIncr;

		anchors.verticalCenter: parent.verticalCenter;

		width: 24;
		height: 20;

		decorator: buttonDecorator;

		enabled: listModel.count > 1 && paginationContainer.currentIndex != paginationContainer.pagesSize - 1;

		iconSource: buttonIncr.enabled ? "../../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal):
										 "../../../../" + Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled);

		onClicked: {
			if (paginationContainer.currentIndex < paginationContainer.pagesSize - 1){
				paginationContainer.currentIndex++;
			}
		}
	}

	Row {
		anchors.verticalCenter: parent.verticalCenter;

		spacing: 4;

		BaseText {
			anchors.verticalCenter: parent.verticalCenter;
			text: qsTr("Per page: ")
		}

		Button {
			id: count25;

			anchors.verticalCenter: parent.verticalCenter;

			width: 25;
			height: 20;

			decorator: buttonDecorator;

			enabled: paginationContainer.countElements != 25;

			text: "25";

			onClicked: {
				paginationContainer.countElements = 25;
			}
		}

		Button {
			id: count50;

			anchors.verticalCenter: parent.verticalCenter;

			width: 25;
			height: 20;

			decorator: buttonDecorator;

			enabled: paginationContainer.countElements != 50;

			text: "50";

			onClicked: {
				paginationContainer.countElements = 50;
			}
		}

		Item {

		}

		Button {
			id: count100;

			anchors.verticalCenter: parent.verticalCenter;

			width: 50;
			height: 20;

			decorator: buttonDecorator;

			enabled: paginationContainer.countElements != 100;

			text: "100";

			onClicked: {
				paginationContainer.countElements = 100;
			}
		}

		Button {
			id: count250;

			anchors.verticalCenter: parent.verticalCenter;

			width: 50;
			height: 20;

			decorator: buttonDecorator;

			enabled: paginationContainer.countElements != 250;

			text: "250";

			onClicked: {
				paginationContainer.countElements = 250;
			}
		}
	}
}


