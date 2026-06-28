import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtbaseSearchSdl 1.0
import imtcontrols 1.0
import imtgui 1.0

Item {
	id: root;
	
	property SearchResults searchResults;
	property var categories: null
	property var activeItems: null
	property int activeTabIndex: -1
	property int activeTotal: 0
	property bool loadingMore: false
	property int contentWidth: Style.sizeHintXXL;
	property string searchText: ""
	
	signal itemDoubleClicked(int tabIndex, int itemIndex);
	signal selectionChanged(var selection);
	signal tabSelected(int tabIndex)
	signal requestLoadMore()
	
	onWidthChanged: {
		checkWidth();
	}

	onSearchResultsChanged: {
		updateGui();
	}

	onCategoriesChanged: {
		updateGui();
	}

	onActiveItemsChanged: {
		if (activeItems){
			table.elements = activeItems
		}
	}

	onActiveTabIndexChanged: {
		// Prefer explicitly loaded paged items for current active tab.
		// In discovery response, first group may have initial items.
		if (activeItems){
			table.elements = activeItems
			return
		}
		if (activeTabIndex >= 0 && categories && categories.m_searchResults){
			let catList = categories.m_searchResults
			if (activeTabIndex < catList.count){
				let c = catList.get(activeTabIndex).item
				if (c && c.m_items){
					table.elements = c.m_items
				}
			}
		}
	}
	
	function checkWidth(){
		if (width - 2 * Style.marginM <= root.contentWidth){
			table.width = width - 2 * Style.marginM;
		}
		else{
			table.width = root.contentWidth;
		}
	}
	
	function updateGui(){
		let dataSource = categories ? categories : searchResults
		if (!dataSource){
			tabPanelContainer.selectedIndex = -1;
			tabPanelContainer.visible = false;
			return;
		}

		tabPanelContainer.selectedIndex = -1;
		let count = dataSource.m_searchResults.count;
		tabPanelContainer.visible = count > 0;
		
		if (count > 0){
			tabPanelContainer.model = dataSource.m_searchResults
			let startIdx = (root.activeTabIndex >= 0 ? root.activeTabIndex : 0)
			tabPanelContainer.selectedIndex = startIdx;
		}
	}
	
	function startLoading(){
		loading.start()
	}
	
	function stopLoading(){
		loading.stop()
	}

	Rectangle {
		id: background
		anchors.fill: parent
		color: Style.baseColor
	}

	Rectangle {
		id: noResultsRect;
		anchors.fill: parent;
		color: Style.baseColor;
		visible: !tabPanelContainer.visible && !root.loadingMore;
		
		// Improved No Results state with icon
		Column {
			id: noResultsContent;
			anchors.centerIn: parent;
			spacing: Style.marginL;
			
			Image {
				id: noResultIcon;
				anchors.horizontalCenter: parent.horizontalCenter;
				width: Style.iconSizeXL;
				height: width;
				source: "../../../" + Style.getIconPath("Icons/Search", Icon.State.Off, Icon.Mode.Disabled);
				sourceSize.width: width;
				sourceSize.height: height;
			}
			
			BaseText {
				anchors.horizontalCenter: parent.horizontalCenter;
				text: root.searchText !== "" ? (qsTr("No results for \"") + root.searchText + "\"") : qsTr("No results");
				font.family: Style.fontFamilyBold;
				font.bold: true;
				font.pixelSize: Style.fontSizeL;
			}
			
			BaseText {
				anchors.horizontalCenter: parent.horizontalCenter;
				text: qsTr("Try adjusting your search query");
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSizeM;
				color: Style.inactiveTextColor;
				visible: root.searchText !== "";
			}
		}
	}

	TabPanel {
		id: tabPanelContainer;
		anchors.top: parent.top;
		anchors.topMargin: Style.marginXL;
		anchors.horizontalCenter: parent.horizontalCenter;
		maxWidth: parent.width
		height: Style.controlHeightM;
		visible: false;
		spacing: Style.marginXS;
		color: Style.baseColor;
		selectedIndex: -1;
		onSelectedIndexChanged: {
			if (selectedIndex >= 0){
				table.resetSelection();
				root.tabSelected(selectedIndex)
				
				// Prefer active paged items (loaded via explicit per-group request).
				// Fallback only for legacy paths.
				if (root.activeItems){
					table.elements = root.activeItems
				} else {
					let src = (root.categories ? root.categories : root.searchResults)
					if (src && src.m_searchResults && selectedIndex < src.m_searchResults.count){
						let searchItem = src.m_searchResults.get(selectedIndex).item
						if (searchItem && searchItem.m_items){
							table.elements = searchItem.m_items
						}
					}
				}
			}
		}
		onLeftClicked: {
			if (selectedIndex > 0){
				selectedIndex--;
				tabPanelContainer.viewTabInListView(selectedIndex);
			}
		}
		onRightClicked: {
			let count = root.categories && root.categories.m_searchResults ? root.categories.m_searchResults.count : 0;
			if (selectedIndex < count - 1){
				selectedIndex++;
				tabPanelContainer.viewTabInListView(selectedIndex);
			}
		}
		tabDelegate:  Component { TabDelegate {
				height: tabPanelContainer.height;

				tabPanel: tabPanelContainer;
				isCloseEnable: tabPanelContainer.isCloseEnable;
				// listView: ;
				decorator: tabPanelContainer.tabDelegateDecorator;
				text: (model.item && model.item.m_name ? model.item.m_name : "") + (model.item && model.item.m_totalCount && model.item.m_totalCount > 0 ? (" (" + model.item.m_totalCount + ")") : "")

				onCloseSignal: {
					tabPanelContainer.closeItem(model.index);
				}
			} }
		tabDelegateDecorator: Component {
			DecoratorBase {
				id: tabPanelDecorator;
				
				width: text.width + 2 * Style.marginXS;
				height: baseElement ? baseElement.height : Style.headerHeight
				
				Row {
					id: content;
					anchors.centerIn: parent;
					height: tabPanelDecorator.height;
					spacing: Style.marginM;
					
					Text {
						id: text;
						anchors.verticalCenter: content.verticalCenter;
						color: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.selected ? Style.textSelectedColor : Style.textColor;
						font.family: Style.fontFamily;
						font.pixelSize: Style.fontSizeM;
						font.bold: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.selected;
						text: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.text : "";
						elide: Text.ElideRight;
					}
				}
				
				// only blue strip for active tab, with radius like item cards
				Rectangle {
					anchors.bottom: parent.bottom;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: text.width + Style.marginM;
					height: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.selected ? 3 : 0;
					color: Style.textSelectedColor;
					radius: Style.radiusM;
				}
			}
		}
	}
	
	Table {
		id: table;
		anchors.top: tabPanelContainer.bottom;
		anchors.topMargin: Style.marginXL;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.marginM;
		anchors.horizontalCenter: parent.horizontalCenter;
		width: root.contentWidth;
		visible: tabPanelContainer.visible;
		showHeaders: false;
		backgroundElementsColor: Style.baseColor;
		elementsSpacing: Style.marginM;
		enableAlternating: false;
		itemHeight: 70;
		rowDelegate: Component {
			TableRowDelegateBase {
				id: tableDelegate;
				tableItem: table
				width: table.elementsListWidth - Style.marginM;
				height: elementView.height;
				selectedOpacity: 0;
				hoverOpacity: 0;
				
				Component.onCompleted: {
					tableDelegate.mouseArea.cursorShape = Qt.PointingHandCursor;
				}
				
				ElementView {
					id: elementView;
					z: tableDelegate.z + 1;
					width: tableDelegate.width;
					name: model.item.m_resultName;
					description: model.item.m_resultDescription;
					color: tableDelegate.mouseArea.containsMouse || tableDelegate.selected ? Style.alternateBaseColor : Style.baseColor;
					controlComp: button;
					
					Component {
						id: button;
						ToolButton {
							width: Style.buttonWidthS;
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
			root.itemDoubleClicked(tabPanelContainer.selectedIndex, index);
		}
	}

	// Scroll-to-load more for current tab (attached to internal list of Table)
	Connections {
		target: table.elementsList
		function onContentYChanged() {
			if (!root.loadingMore && root.activeTotal > 0){
				let loaded = (root.activeItems && root.activeItems.count) ? root.activeItems.count : 0
				if (loaded >= root.activeTotal){
					return
				}
				let y = table.elementsList.contentY
				let h = table.elementsList.height
				let ch = table.elementsList.contentHeight
				if (ch > 0 && (y + h > ch - 180)){
					root.requestLoadMore()
				}
			}
		}
	}

	// Bottom "loading more" indicator (simple, only when appending)
	Item {
		anchors.left: table.left
		anchors.right: table.right
		anchors.top: table.bottom
		anchors.topMargin: Style.marginS
		height: root.loadingMore ? Style.controlHeightS : 0
		visible: root.loadingMore

		BaseText {
			anchors.centerIn: parent
			text: qsTr("Loading more...")
			font.pixelSize: Style.fontSizeS;
			color: Style.inactiveTextColor;
		}
	}
	
	Loading {
		id: loading;
		anchors.fill: parent;
		background.color: Style.baseColor
		visible: false;
	}
}


