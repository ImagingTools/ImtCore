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

	// Result categories as centered toggle buttons, the same as Dashboard / User Actions / Analytics on Workspace.
	Item {
		id: tabPanelContainer;
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: visible ? Style.controlHeightM + 2 * Style.marginM : 0;
		visible: false;
		clip: true;

		property int selectedIndex: -1;
		property var model: null;

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
		Row {
			anchors.centerIn: parent;
			spacing: Style.spacingS;

			Repeater {
				model: tabPanelContainer.model;

				delegate: Button {
					id: categoryButton;
					// Tests address the categories as Tab0, Tab1, ...
					objectName: "Tab" + model.index;

					height: Style.controlHeightM;
					widthFromDecorator: true;
					checked: tabPanelContainer.selectedIndex === model.index;
					text: (model.item && model.item.m_name ? model.item.m_name : "") + (model.item && model.item.m_totalCount && model.item.m_totalCount > 0 ? (" (" + model.item.m_totalCount + ")") : "");

					decorator: Component {
						TopButtonDecorator {
							color: !baseElement ? "transparent" : baseElement.checked ? Style.selectedColor : baseElement.mouseArea.containsMouse ? Style.toolButtonHoverColor : "transparent";
							textColor: Style.textColor;
							border.width: 0;
							radius: Style.buttonRadius;
						}
					}

					onClicked: {
						tabPanelContainer.selectedIndex = model.index;
					}
				}
			}
		}

		Rectangle {
			anchors.bottom: parent.bottom;
			width: parent.width;
			height: 1;
			color: Style.borderColor;
		}
	}
	
	Text {
		id: resultsSummary;
		anchors.top: tabPanelContainer.bottom;
		anchors.topMargin: Style.marginM;
		anchors.left: table.left;
		anchors.leftMargin: Style.marginM;
		visible: tabPanelContainer.visible && root.activeTotal > 0;
		height: visible ? implicitHeight : 0;
		text: root.activeTotal === 1 ? qsTr("1 result") : qsTr("%1 results").arg(root.activeTotal);
		font.family: Style.fontFamily;
		font.pixelSize: Style.fontSizeS;
		color: Style.subtitleColor;
	}

	// A plain list in the VS Code sense: rows without frames, hover and selection washes,
	// the open action shown only on the row in use.
	Table {
		id: table;
		anchors.top: resultsSummary.bottom;
		anchors.topMargin: Style.spacingS;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.marginM;
		anchors.horizontalCenter: parent.horizontalCenter;
		width: root.contentWidth;
		visible: tabPanelContainer.visible;
		showHeaders: false;
		backgroundElementsColor: Style.baseColor;
		elementsSpacing: Style.spacingXXS;
		enableAlternating: false;
		itemHeight: Style.controlHeightL + Style.marginM;
		rowDelegate: Component {
			TableRowDelegateBase {
				id: tableDelegate;
				tableItem: table
				width: table.elementsListWidth - Style.marginM;
				height: table.itemHeight;
				selectedOpacity: 0;
				hoverOpacity: 0;
				selectedColor: "transparent";
				hoverColor: "transparent";
				focusOutlineVisible: false;

				Component.onCompleted: {
					tableDelegate.mouseArea.cursorShape = Qt.PointingHandCursor;
				}

				Rectangle {
					id: resultRow;
					z: tableDelegate.z + 1;
					anchors.fill: parent;
					radius: Style.radiusM;
					color: tableDelegate.selected ? Style.selectedColor : tableDelegate.mouseArea.containsMouse ? Style.hover : "transparent";
					border.width: tableDelegate.selected ? 1 : 0;
					border.color: Style.focusRingColor;

					Column {
						anchors.left: parent.left;
						anchors.leftMargin: Style.marginM;
						anchors.right: openButton.left;
						anchors.rightMargin: Style.marginS;
						anchors.verticalCenter: parent.verticalCenter;
						spacing: Style.spacingXXS;

						Text {
							width: parent.width;
							text: model.item.m_resultName;
							font.family: Style.fontFamily;
							font.pixelSize: Style.fontSizeM;
							color: Style.textColor;
							elide: Text.ElideRight;
						}

						Text {
							width: parent.width;
							visible: text !== "";
							text: model.item.m_resultDescription ? model.item.m_resultDescription : "";
							font.family: Style.fontFamily;
							font.pixelSize: Style.fontSizeS;
							color: Style.subtitleColor;
							elide: Text.ElideRight;
						}
					}

					ToolButton {
						id: openButton;
						anchors.right: parent.right;
						anchors.rightMargin: Style.marginS;
						anchors.verticalCenter: parent.verticalCenter;
						width: Style.controlHeightS;
						height: width;
						visible: tableDelegate.selected || tableDelegate.mouseArea.containsMouse;
						iconSource: "../../../" + Style.getIconPath("Icons/ExternalLink", Icon.State.On, Icon.Mode.Normal);
						tooltipText: qsTr("Go to the content");
						decorator: Component {
							ToolButtonDecorator {
								radius: Style.buttonRadius;
								icon.width: Style.iconSizeS;
							}
						}
						onClicked: {
							table.select(model.index);
							table.doubleClicked("", model.index)
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
	
	SkeletonTableView {
		id: loading;
		anchors.fill: parent;
		headerVisible: false;
		cardRows: true;
		rowHeight: table.itemHeight;
		rowSpacing: table.elementsSpacing;
		contentMaxWidth: table.width;
		contentTopMargin: Style.controlHeightM + 2 * Style.marginM + Style.marginM;
	}
}


