import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Row {
	id: paginationContainer;

	height: visible ? Style.controlHeightS : 0;
	spacing: Style.spacingL;

	property int pagesSize: 1;
	property int currentValue: currentIndex + 1;
	property int currentIndex: 0;

	property color color: Style.baseColor;

	// The number of elements on the page
	property int countElements: 25;

	property int countAllElements: 0;

	property alias pageCount: listModel.count;
	property bool perPageTitleVisible: false;

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

		height: Style.buttonHeightXS;
		width: Style.buttonWidthM;

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

		spacing: Style.marginXS;

		Repeater {
			id: repeaterPagination;

			delegate: Component {Button {
					id: buttonDelegate;

					width: Math.max(Style.buttonWidthM, helperText.width + 8);
					height: Style.buttonHeightXS;

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

		width: Style.buttonWidthM;
		height: Style.buttonHeightXS;

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
			visible: paginationContainer.perPageTitleVisible;
			anchors.verticalCenter: parent.verticalCenter;
			text: qsTr("Per page: ")
		}
		
		TreeItemModel {
			id: perPagemodel
			Component.onCompleted: {
				let index = insertNewItem()
				setData("id", "25", index)
				setData("name", "25", index)

				index = insertNewItem()
				setData("id", "50", index)
				setData("name", "50", index)

				index = insertNewItem()
				setData("id", "100", index)
				setData("name", "100", index)
				
				index = insertNewItem()
				setData("id", "250", index)
				setData("name", "250", index)
				
				index = insertNewItem()
				setData("id", "500", index)
				setData("name", "500", index)
				
				comboBox.model = perPagemodel
				comboBox.currentIndex = 0
			}
		}
		
		ComboBox {
			id: comboBox
			width: 65
			height: Style.buttonHeightXS

			onCurrentIndexChanged: {
				if (currentIndex >= 0){
					let itemId = model.getData("id", currentIndex)
					paginationContainer.countElements = Number(itemId)
				}
			}
		}
	}
}


