import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Row {
	id: paginationContainer;

	// Test instrumentation (Tests/ProLifeGui). Inert.
	objectName: "Pagination";

	height: visible ? Style.controlHeightS : 0;
	spacing: Style.spacingS;

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

	// Toolbar-style buttons, as in VS Code: no fill at rest, a wash under the pointer.
	Component {
		id: buttonDecorator;

		ButtonDecorator {
			color: baseElement && baseElement.enabled && baseElement.hovered ? Style.toolButtonHoverColor : "transparent";
			border.width: 0;
			radius: Style.buttonRadius;
			icon.width: Style.iconSizeS;
			icon.height: Style.iconSizeS;
		}
	}

	// A page number: the current page carries the selection wash and a bolder figure.
	Component {
		id: pageButtonDecorator;

		ButtonDecorator {
			color: !baseElement ? "transparent"
				 : baseElement.isCurrent ? Style.selectedColor
				 : baseElement.enabled && baseElement.hovered ? Style.toolButtonHoverColor : "transparent";
			border.width: 0;
			radius: Style.buttonRadius;
			textColor: baseElement && (baseElement.enabled || baseElement.isCurrent) ? Style.textColor : Style.subtitleColor;
			font.bold: baseElement ? baseElement.isCurrent : false;
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

		color: Style.subtitleColor;
		font.pixelSize: Style.fontSizeS;
		// Concatenated: chained arg() substitutes only the first placeholder in the web runtime.
		text: paginationContainer.countAllElements === 0 ? qsTr("No items") :
			  (paginationContainer.currentIndex * paginationContainer.countElements + 1) + "–"
			  + ((paginationContainer.pagesSize - 1 != paginationContainer.currentIndex) ?
					 paginationContainer.currentValue * paginationContainer.countElements :
					 paginationContainer.countAllElements)
			  + " " + qsTr("of") + " " + paginationContainer.countAllElements
	}

	// Gap between the range and the page controls.
	Item {
		width: Style.marginS;
		height: 1;
	}

	Button {
		id: buttonDecr;
		objectName: "PrevPageButton";

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

		spacing: Style.spacingXXS;

		Repeater {
			id: repeaterPagination;

			delegate: Component {Button {
					id: buttonDelegate;

					property bool isCurrent: model.number !== -1 && paginationContainer.currentIndex === model.number - 1;

					width: Math.max(Style.buttonWidthM, helperText.width + Style.marginM);
					height: Style.buttonHeightXS;

					decorator: pageButtonDecorator;

					enabled: model.number !== -1 && !buttonDelegate.isCurrent;
					text: model.number === -1 ? "…" : model.number;

					onClicked: {
						paginationContainer.currentIndex = model.number - 1;
					}

					BaseText {
						id: helperText;
						text: buttonDelegate.text;
						font.bold: true;
						visible: false;
					}
				}
			}
		}
	}

	Button {
		id: buttonIncr;
		objectName: "NextPageButton";

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

		spacing: Style.spacingS;
		leftPadding: Style.marginS;

		BaseText {
			visible: paginationContainer.perPageTitleVisible;
			anchors.verticalCenter: parent.verticalCenter;
			color: Style.subtitleColor;
			font.pixelSize: Style.fontSizeS;
			text: qsTr("Per page:")
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
			objectName: "PageSizeCombo"
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


