import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0

ViewBase {
	id: collectionViewBaseContainer;
	
	clip: true;
	
	property string collectionId;
	
	property alias table: tableInternal;
	property bool hasPagination: true;
	property bool hasFilter: true;
	
	property bool hasSort: true;
	property bool canResetFilters: true;
	property bool headerRightClickEnabled: true;
	property alias isMultiCheckable: tableInternal.isMultiCheckable;
	
	property alias filterMenu: filterMenu_;
	property alias loading: loading_;
	property bool filterMenuVisible: true
	property alias pagination: pagination_;
	property alias elementsCount: tableInternal.elementsCount;
	property alias tableRowDelegate: tableInternal.rowDelegate
	property alias scrollbarItem: tableInternal.scrollbarItem
	property alias scrollbarItemHoriz: tableInternal.scrollbarItemHoriz

	property CollectionFilter collectionFilter: CollectionFilter {}
	property DocCollectionFilter documentCollectionFilter: DocCollectionFilter {}
	property IdSelectionManager selectionManager: IdSelectionManager {}

	property Component dataControllerComp: Component{CollectionDataController{}}
	property var dataController: null;
	
	signal selectedIndexChanged(int index);
	signal tableViewParamsAccepted();
	
	signal elementsChanged();
	signal headersChanged();
	signal tableDecoratorChanged()

	signal filterChanged(string filterId, var filterValue);
	
	signal selectionChanged(var ids, var indexes);
	signal checkedItemsChanged();
	signal rightButtonMouseClicked(int mouseX, int mouseY);
	signal doubleClicked(string id, int index);

	property bool activeFilter: false
	property int activeFilterCount: 0
	property int contentHeight: filterArea.height + (tableInternal.headerHeight + tableInternal.contentHeight) + paginationObj.height
	
	Component.onCompleted: {
		filterArea.height = collectionViewBaseContainer.filterMenuVisible ? filterArea.expandedHeight : 0;

		tableInternal.focus = true;
	}

	QtObject {
		id: internal
		function checkActiveFilter(){
			collectionViewBaseContainer.activeFilter = collectionViewBaseContainer.hasActiveFilter() || collectionViewBaseContainer.filterMenu.hasActiveFilter()

			let count = collectionViewBaseContainer.filterMenu.activeFilterCount()
			let collectionFilter = collectionViewBaseContainer.collectionFilter
			if (collectionFilter && collectionFilter.getTextFilter() !== ""){
				count = count + 1
			}

			collectionViewBaseContainer.activeFilterCount = count
		}
	}
	
	function onModelChanged(){}
	
	function getSelectedIds(){
		return selectionManager.selectedIds
	}
	
	function hasActiveFilter(){
		if (collectionFilter){
			return !collectionFilter.isEmpty()
		}
		
		return false
	}
	
	function registerFieldFilterDelegate(filterId, filterDelegateComp){
		filterMenu_.registerFieldFilterDelegate(filterId, filterDelegateComp)
	}

	function setFilterDependency(filterId, dependsOnFilterId){
		filterMenu_.setFilterDependency(filterId, dependsOnFilterId)
	}

	Item {
		id: filterArea;

		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;

		height: 0;
		clip: true;

		readonly property int expandedHeight: filterMenu_.height + 2 * Style.marginM;

		onExpandedHeightChanged: {
			if (collectionViewBaseContainer.filterMenuVisible && !filterAreaAnimation.running){
				filterArea.height = filterArea.expandedHeight;
			}
		}

	FilterMenu {
		id: filterMenu_;
		anchors.top: parent.top;
		anchors.topMargin: Style.marginM;
		anchors.left: parent.left;
		anchors.right: parent.right;
		canResetFilters: collectionViewBaseContainer.canResetFilters
		complexFilter: collectionViewBaseContainer.collectionFilter;
		documentFilter: collectionViewBaseContainer.documentCollectionFilter;
		onClose: {
			collectionViewBaseContainer.filterMenuVisible = false;
		}

		onFilterChanged: {
			let collectioFilter = collectionViewBaseContainer.collectionFilter
			if (collectioFilter){
				let sortingInfo = collectioFilter.getSortingInfo()
				if (sortingInfo){
					tableInternal.setSortingInfo(sortingInfo.m_fieldId, sortingInfo.m_sortingOrder)
				}
			}

			internal.checkActiveFilter()
			collectionViewBaseContainer.doUpdateGui();
		}

		onClearAllFilters: {
			if (collectionViewBaseContainer.collectionFilter){
				collectionViewBaseContainer.collectionFilter.clearAllFilters(beQuiet)
			}

			if (collectionViewBaseContainer.documentCollectionFilter){
				collectionViewBaseContainer.documentCollectionFilter.clear(beQuiet)
			}

			tableInternal.clearSortingInfo(beQuiet)
		}
	}
	}

	NumberAnimation {
		id: filterAreaAnimation;

		target: filterArea;
		property: "height";
		duration: 160;
		easing.type: Easing.OutCubic;
	}

	onFilterMenuVisibleChanged: {
		filterAreaAnimation.from = filterArea.height;
		filterAreaAnimation.to = collectionViewBaseContainer.filterMenuVisible ? filterArea.expandedHeight : 0;
		filterAreaAnimation.restart();
	}

	onHeadersChanged: {
		table.tableViewParams.clear();
		
		for (let i = 0; i < table.headers.getItemsCount(); i++){
			let headerId = table.getHeaderId(i);
			if (!table.tableViewParams.headerIsExists(headerId)){
				table.tableViewParams.setHeaderSize(headerId, -1);
				table.tableViewParams.setHeaderVisible(headerId, true);
				table.tableViewParams.setHeaderOrder(headerId, i);
			}
		}
	}
	
	Rectangle {
		id: backgroundTable;
		anchors.top: filterArea.bottom;
		anchors.left: parent.left;
		anchors.topMargin: 0;
		anchors.bottom: paginationObj.top;
		width: tableInternal.minWidth * tableInternal.columnCount < parent.width ? tableInternal.minWidth * tableInternal.columnCount : parent.width;
		color: Style.baseColor;
		
		TreeItemModel{
			id: tableDecoratorModel;
			
			property bool compl: false;
			property bool complCompl: tableDecoratorModel.compl && loaderTableDecorator.compl && tableInternal.headersCompl;
			
			Component.onCompleted: {
				tableDecoratorModel.compl = true;
			}
			
			onComplComplChanged: {
				if(tableDecoratorModel.complCompl){
					
					var tableDecorator = loaderTableDecorator.item;
					var count = tableInternal.headers.getItemsCount();
					
					if(tableDecorator.getItemsCount()){
						
						var headers = tableDecoratorModel.addTreeModel("Headers");
						var cells = tableDecoratorModel.addTreeModel("Cells");
						var cellWidth = tableDecoratorModel.addTreeModel("CellWidth");
						
						var general;
						if(tableDecorator.isValidData("General")){
							general = tableDecorator.getTreeItemModel("General")
							let keys = general.getKeys();
							for(let i = 0; i < keys.length; i++){
								if(tableInternal[keys[i]] !== undefined){
									tableInternal[keys[i]] = general.getData(keys[i]);
								}
							}
						}
						
						for(let i = 0; i < count; i++){
							headers.insertNewItem();
							cells.insertNewItem();
							cellWidth.insertNewItem();
							headers.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("Headers"));
							cells.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("Cells"));
							//cellWidth.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("CellWidth"));
							var widthVal = tableDecorator.getTreeItemModel("CellWidth").isValidData("Width",i) ?
										tableDecorator.getTreeItemModel("CellWidth").getData("Width",i) : -1;
							cellWidth.setData("Width", widthVal,i);
							
						}
						
						tableInternal.tableDecorator = tableDecoratorModel;
					}
				}
			}
		}
		
		Loader{
			id: loaderTableDecorator;
			sourceComponent: Style.collectionTableDecorator;
			
			property bool compl: false;
			
			onLoaded: {
				if(loaderTableDecorator.item){
					loaderTableDecorator.compl = true;
				}
			}
		}
		
		Table {
			id: tableInternal;
			anchors.left: parent.left;
			anchors.right: tableRightPanel.left;
			anchors.top: parent.top;
			anchors.bottom: parent.bottom;
			hasFilter: collectionViewBaseContainer.hasFilter;
			hasSort: collectionViewBaseContainer.hasSort;
			scrollbarVisible: false;
			canSetBorderParams: true;
			canSetBorderParams_deleg: true;
			canMoveColumns: true;
			scrollbarRightMargin: collectionViewBaseContainer.hasFilter ? -20 : 0;
			verticalBorderHeight: headerHeight / 2;
			verticalBorderSize: 1;
			visibleLeftBorderFirst: false;
			borderColorVertical: Style.borderColor;
			
			property real minWidth: 1000000;
			property bool headersCompl: false;
			property bool selectionBlock: false
			
			onSelectionChanged: {
				if (selectionBlock){
					return
				}

				let count = !elements ? 0 : elements.getItemsCount()
				if (count > 0){
					let selectedIds = []
					for (let i = 0; i < selection.length; i++){
						let elementId = elements.getData("id", selection[i])
						selectedIds.push(elementId)
					}
					collectionViewBaseContainer.selectionManager.selectMultiple(selectedIds)
				}
				else{
					collectionViewBaseContainer.selectionManager.clear()
				}
				
				collectionViewBaseContainer.selectionChanged(collectionViewBaseContainer.selectionManager.selectedIds, selectionManager.selectedIndexes);
			}
			
			onCheckedItemsChanged: {
				collectionViewBaseContainer.checkedItemsChanged();
			}
			
			onElementsChanged: {
				selectionBlock = true
				resetSelection()

				let elementsModel = collectionViewBaseContainer.dataController.elementsModel
				let count = elementsModel.getItemsCount()
				let selectedIndexes = []
				for (let i = 0; i < count; i++){
					let elementId = elementsModel.getData("id", i)
					if (collectionViewBaseContainer.selectionManager.isSelected(elementId)){
						selectedIndexes.push(i)
					}
				}

				selectionManager.selectMultiple(selectedIndexes)
				
				selectionBlock = false

				collectionViewBaseContainer.elementsChanged();
			}
			
			onHeadersChanged: {
				collectionViewBaseContainer.headersChanged();
				if(tableInternal.headers.getItemsCount()){
					tableInternal.headersCompl = true;
				}
			}

			onTableDecoratorChanged: {
				collectionViewBaseContainer.tableDecoratorChanged();
			}
			
			onHeaderRightMouseClicked: {
				if(collectionViewBaseContainer.headerRightClickEnabled){
					ModalDialogManager.openDialog(tableHeaderParamComp, {});
				}
			}
			
			onRightButtonMouseClicked: {
				collectionViewBaseContainer.rightButtonMouseClicked(mouseX, mouseY);
			}
			
			onDoubleClicked: {
				collectionViewBaseContainer.doubleClicked(id, index);
			}
			
			onSortingChanged: {
				if (collectionViewBaseContainer.collectionFilter){
					collectionViewBaseContainer.collectionFilter.setSortingInfo(headerId, sortOrder);
					collectionViewBaseContainer.collectionFilter.filterChanged();
				}
			}
		}
		

		// Nothing in the list, said plainly. An empty table used to be a header
		// with blank space under it, which reads the same whether the collection
		// has nothing in it or the filters have hidden everything - and those two
		// want completely different things from the reader.
		Column {
			id: emptyState;

			anchors.left: parent.left;
			anchors.right: tableRightPanel.left;
			anchors.verticalCenter: parent.verticalCenter;
			anchors.verticalCenterOffset: tableInternal.headerHeight / 2;

			spacing: Style.marginM;

			visible: tableInternal.elementsCount === 0 && !loading_.visible;

			readonly property bool filtered: collectionViewBaseContainer.activeFilterCount > 0;

			Image {
				anchors.horizontalCenter: parent.horizontalCenter;

				width: Style.iconSizeL;
				height: width;
				sourceSize.width: width;
				sourceSize.height: height;
				source: "qrc:/" + Style.getIconPath(emptyState.filtered ? "Icons/Filter" : "Icons/Table",
					Icon.State.On, Icon.Mode.Disabled);
			}

			BaseText {
				anchors.horizontalCenter: parent.horizontalCenter;

				width: Math.min(parent.width - 2 * Style.marginXL, Style.sizeHintM);
				horizontalAlignment: Text.AlignHCenter;
				text: emptyState.filtered ? qsTr("Nothing matches the filters") : qsTr("Nothing here yet");
				font.pixelSize: Style.fontSizeL;
				color: Style.textColor;
				wrapMode: Text.WordWrap;
			}

			BaseText {
				anchors.horizontalCenter: parent.horizontalCenter;

				width: Math.min(parent.width - 2 * Style.marginXL, Style.sizeHintM);
				horizontalAlignment: Text.AlignHCenter;
				text: emptyState.filtered
					? qsTr("%1 filters are narrowing this list down.").arg(collectionViewBaseContainer.activeFilterCount)
					: qsTr("Items added to this collection will show up here.");
				color: Style.subtitleColor;
				wrapMode: Text.WordWrap;
			}

			// Offered only when the filters are what emptied the list - the way
			// back out, without hunting for the panel first.
			ToolbarButton {
				anchors.horizontalCenter: parent.horizontalCenter;

				visible: emptyState.filtered;
				text: qsTr("Clear all filters");
				iconSource: "qrc:/" + Style.getIconPath("Icons/FilterReset", Icon.State.On, Icon.Mode.Normal);

				onClicked: {
					collectionViewBaseContainer.filterMenu.clearAllFilters(true);
					collectionViewBaseContainer.filterMenu.filterChanged();
				}
			}
		}
		Item {
			id: tableRightPanel;
			anchors.top: parent.top;
			anchors.right: parent.right;
			anchors.bottom: parent.bottom;
			width: collectionViewBaseContainer.hasFilter ? 35 : 0;
			visible: tableInternal.width > 0;
			
			Item {
				id: filterItem;
				anchors.top: parent.top;
				width: parent.width;
				height: tableInternal.headerHeight;

				// Open or shut, and how many filters are on: two different things,
				// which the button used to conflate into one icon that changed
				// shade. The panel being open is the tinted disc; the filters that
				// are on are the number on the badge - the only thing left saying
				// so once the panel is shut.
				ToolButton {
					id: iconFilter;
					objectName: "FilterVisible"
					anchors.centerIn: parent;
					width: Style.buttonWidthL;
					height: width;
					visible: collectionViewBaseContainer.hasFilter;
					// Named by the card below rather than by the stock tooltip: this
					// button lives against the right edge, and a tooltip that opens
					// rightwards has nowhere to go.
					// A plain funnel, in both states. It used to be a funnel with a
					// pencil when shut - which reads as "edit a filter" - and a funnel
					// with a tick when something was on, a job the count badge now does
					// properly. The funnel says "filters"; the disc behind it says the
					// panel is open; the badge says how many are working.
					icon.source: "qrc:/" + Style.getIconPath("Icons/Filter", Icon.State.On,
						collectionViewBaseContainer.activeFilter ? Icon.Mode.Selected : Icon.Mode.Normal)
					onClicked: {
						collectionViewBaseContainer.filterMenuVisible = !collectionViewBaseContainer.filterMenuVisible;
					}
					decorator: Component {
						ToolButtonDecorator {
							// Matched to the reset button in the panel: same button size,
							// same glyph size, so the pair reads as one control.
							icon.width: Style.iconSizeM + Style.spacingXS
							radius: height / 2
							color: collectionViewBaseContainer.filterMenuVisible ? Style.selectedColor
								: baseElement && baseElement.hovered ? Style.hover : "transparent"
							// A ring while anything is filtering, so the button carries
							// the state even at a glance and even without the badge.
							border.width: collectionViewBaseContainer.activeFilterCount > 0 ? 1 : 0
							border.color: Style.titleColor
						}
					}

					// Above the decorator, which is built after this and would
					// otherwise be painted over it.
					Rectangle {
						anchors.right: parent.right;
						anchors.top: parent.top;

						z: 1;

						width: Math.max(Style.iconSizeXS + Style.spacingXS, badgeText.implicitWidth + Style.spacingXS);
						height: Style.iconSizeXS + Style.spacingXS;
						radius: height / 2;
						visible: collectionViewBaseContainer.activeFilterCount > 0;
						color: Style.titleColor;

						// Filled to the badge and aligned in both directions. Centred
						// by anchor alone the glyph sat off to one side of a circle
						// barely wider than itself.
						BaseText {
							id: badgeText;

							anchors.fill: parent;

							horizontalAlignment: Text.AlignHCenter;
							verticalAlignment: Text.AlignVCenter;
							text: "" + collectionViewBaseContainer.activeFilterCount;
							font.family: Style.fontFamilyBold;
							font.pixelSize: Style.fontSizeXS;
							color: Style.baseColor;
						}
					}
				}

			// Names the button, in the same card the navigation rail uses, but
			// pointing the other way: this button sits against the right edge, so
			// the card comes out to its left with the tip on its own right side.
			Item {
				id: filterHint;

				anchors.right: filterItem.left;
				anchors.rightMargin: filterHint.slide;
				anchors.verticalCenter: filterItem.verticalCenter;

				z: 200;

				width: hintBody.width + Style.spacingS;
				height: Style.controlHeightM;

				visible: filterHint.opacity > 0;
				opacity: 0;

				property real slide: 0;

				Rectangle {
					id: hintArrow;

					// Centred on the body's edge, so the body covers the inner half
					// and only a triangle shows. Centred on the item's edge instead,
					// nothing covered it and the whole square stood there as a lozenge.
					x: hintBody.width - width / 2;
					z: -1;

					anchors.verticalCenter: parent.verticalCenter;

					width: Style.marginM;
					height: width;
					rotation: 45;
					color: hintBody.color;
				}

				Rectangle {
					id: hintBody;

					anchors.left: parent.left;
					anchors.verticalCenter: parent.verticalCenter;

					width: hintLabel.width + 2 * Style.marginM;
					height: parent.height;
					radius: Style.radiusM;
					color: Style.titleColor;

					BaseText {
						id: hintLabel;

						anchors.centerIn: parent;

						text: collectionViewBaseContainer.filterMenuVisible ? qsTr("Hide filters")
							: collectionViewBaseContainer.activeFilterCount > 0
								? qsTr("Show filters - %1 active").arg(collectionViewBaseContainer.activeFilterCount)
								: qsTr("Show filters");
						font.pixelSize: Style.fontSizeM;
						color: Style.baseColor;
					}
				}

				ParallelAnimation {
					id: filterHintIn;

					NumberAnimation {
						target: filterHint;
						property: "opacity";
						to: 1;
						duration: 120;
						easing.type: Easing.OutQuad;
					}

					NumberAnimation {
						target: filterHint;
						property: "slide";
						to: Style.spacingXS;
						duration: 120;
						easing.type: Easing.OutCubic;
					}
				}

				NumberAnimation {
					id: filterHintOut;

					target: filterHint;
					property: "opacity";
					to: 0;
					duration: 90;
					easing.type: Easing.InQuad;
				}

				// Bound, not listened for: a mouse area that switches off never
				// reports the pointer leaving, and a card driven by that signal
				// alone would stay on screen. Read as a condition it goes as soon
				// as the button stops being usable.
				readonly property bool wanted: iconFilter.enabled && iconFilter.visible
					&& iconFilter.mouseArea && iconFilter.mouseArea.containsMouse;

				onWantedChanged: {
					if (filterHint.wanted){
						filterHintOut.stop();
						filterHint.slide = 0;
						filterHintIn.restart();
					}
					else{
						filterHintIn.stop();
						filterHintOut.restart();
					}
				}
			}
			}
		}
	}
	
	Component {
		id: tableHeaderParamComp;
		
		TableHeaderParamComp{
			tableItem: tableInternal;
			onFinished: {
				if (buttonId === Enums.apply || buttonId === Enums.ok){
					if (contentItem){
						collectionViewBaseContainer.tableViewParamsAccepted();
					}
				}
			}
		}
	}
	
	Rectangle {
		id: paginationObj;
		anchors.bottom: parent.bottom;
		anchors.horizontalCenter: parent.horizontalCenter;
		width: parent.width;
		height: !visible ? 0 : Style.controlHeightM;
		color: pagination_.color;
		visible: collectionViewBaseContainer.hasPagination
		
		Pagination {
			id: pagination_;
			anchors.horizontalCenter: parent.horizontalCenter;
			anchors.verticalCenter: parent.verticalCenter;
			pagesSize: 1;

			onCurrentIndexChanged: {
				tableInternal.resetSelection();
				collectionViewBaseContainer.doUpdateGui();
			}
			
			onCountElementsChanged: {
				tableInternal.selectedIndex = -1;
				collectionViewBaseContainer.doUpdateGui();
			}
		}
	}

	Loading {
		id: loading_;
		anchors.top: filterArea.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: paginationObj.bottom
		// baseColor is the Loading default and matches Agents/Services/Messages tables.
		background.color: Style.baseColor
		visible: false;
	}
}
