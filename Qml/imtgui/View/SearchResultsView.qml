import QtQuick 2.12
import Acf 1.0
import imtbaseSearchSdl 1.0
import imtcontrols 1.0
import imtgui 1.0

Item {
	id: root;
	
	property SearchResults searchResults;
	property int contentWidth: 700;
	
	signal itemDoubleClicked(int tabIndex, int itemIndex);
	signal selectionChanged(var selection);
	
	onWidthChanged: {
		checkWidth();
	}

	onSearchResultsChanged: {
		updateGui();
	}
	
	function checkWidth(){
		if (width - 2 * Style.sizeMainMargin <= root.contentWidth){
			table.width = width - 2 * Style.sizeMainMargin;
		}
		else{
			table.width = root.contentWidth;
		}
	}
	
	function updateGui(){
		if (!searchResults){
			tabPanel.selectedIndex = -1;
			tabPanel.visible = false;
			return;
		}

		tabPanel.selectedIndex = -1;
		let count = searchResults.m_searchResults.count;
		tabPanel.visible = count > 0;
		
		if (count > 0){
			tabPanel.model = searchResults.m_searchResults
			tabPanel.selectedIndex = 0;
		}
	}
	
	function startLoading(){
		loading.start()
	}
	
	function stopLoading(){
		loading.stop()
	}
	
	Rectangle {
		id: noResultsRect;
		anchors.fill: parent;
		color: Style.backgroundColor2;
		visible: !tabPanel.visible;
		
		BaseText {
			anchors.centerIn: parent;
			text: qsTr("No results");
			font.family: Style.fontFamilyBold;
			font.bold: true;
		}
	}
	
	TabPanel {
		id: tabPanel;
		anchors.top: parent.top;
		anchors.topMargin: Style.sizeMainMargin;
		anchors.horizontalCenter: parent.horizontalCenter;
		height: 30;
		visible: false;
		spacing: Style.sizeMainMargin;
		color: Style.backgroundColor2;
		selectedIndex: -1;
		onSelectedIndexChanged: {
			if (selectedIndex >= 0){
				table.resetSelection();
				
				let searchItem = root.searchResults.m_searchResults.get(selectedIndex).item
				table.elements = searchItem.m_items;
			}
		}
		tabDelegateDecorator: Component {
			DecoratorBase {
				id: tabPanelDecorator;
				
				width: content.width + 2 * Style.sizeMainMargin;
				height: baseElement ? baseElement.height : 50
				
				Rectangle {
					anchors.fill: tabPanelDecorator;
					color: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.selected ? Style.selectedColor: "transparent";
					radius: 15;
					border.width: 1;
					border.color: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.selected ? Style.iconColorOnSelected: Style.borderColor;
				}
				
				Row {
					id: content;
					anchors.centerIn: tabPanelDecorator;
					height: tabPanelDecorator.height;
					spacing: Style.sizeMainMargin;
					
					Text {
						id: text;
						anchors.verticalCenter: content.verticalCenter;
						color: Style.textColor;
						font.family: Style.fontFamily;
						font.pixelSize: Style.fontSizeNormal;
						text: model.item.m_name;
						elide: Text.ElideRight;
					}
				}
			}
		}
	}
	
	Rectangle {
		id: separator;
		anchors.top: tabPanel.bottom;
		anchors.topMargin: Style.sizeMainMargin;
		width: parent.width;
		height: 1;
		color: Style.borderColor;
	}
	
	Table {
		id: table;
		anchors.top: separator.bottom;
		anchors.topMargin: Style.sizeMainMargin;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.sizeMainMargin;
		anchors.horizontalCenter: parent.horizontalCenter;
		width: root.contentWidth;
		visible: tabPanel.visible;
		showHeaders: false;
		backgroundElementsColor: Style.backgroundColor2;
		elementsSpacing: Style.sizeMainMargin;
		enableAlternating: false;
		itemHeight: 70;
		rowDelegate: Component {
			TableRowDelegateBase {
				id: tableDelegate;
				tableItem: table
				width: table.elementsListWidth - Style.sizeMainMargin;
				height: elementView.height;
				selectedOpacity: 0;
				hoverOpacity: 0;
				
				ElementView {
					id: elementView;
					z: tableDelegate.z + 1;
					width: tableDelegate.width;
					name: model.item.m_resultName;
					description: model.item.m_resultDescription;
					color: tableDelegate.mouseArea.containsMouse || tableDelegate.selected ? Style.selectedColor : Style.baseColor;
					controlComp: tableDelegate.mouseArea.containsMouse ? button : undefined;
					
					Component {
						id: button;
						ToolButton {
							width: 20;
							height: width;
							iconSource: "../../../" + Style.getIconPath("Icons/ExternalLink", Icon.State.On, Icon.Mode.Normal);
							tooltipText: qsTr("Go to the content");
							onClicked: {
								table.select(model.index);
								table.doubleClicked("", model.index)
							}
						}
					}
				}
			}
		}
		
		onSelectionChanged: {
			root.selectionChanged(selection);
		}
		
		onDoubleClicked: {
			root.itemDoubleClicked(tabPanel.selectedIndex, index);
		}
	}
	
	Loading {
		id: loading;
		anchors.fill: parent;
		visible: false;
	}
}


