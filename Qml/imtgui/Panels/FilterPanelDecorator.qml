import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

DecoratorBase {
	id: filterPanelDecorator;
	
	// width: customFiltersFlickable.width + mainFilters.width + clearAllButton.width
	height: content.height;
	
	property alias contentWidth: content.width;
	
	property CollectionFilter complexFilter: baseElement ? baseElement.complexFilter : null;
	property DocCollectionFilter documentFilter: baseElement ? baseElement.documentFilter : null;
	property bool canResetFilters: baseElement ? baseElement.canResetFilters : false
	property bool hasDateFilter: true
	property bool hasDocumentFilter: true

	property alias searchTextInputWidth: tfc.width;
	property alias clearAllButtonWidth: clearAllButton.width;


	onBaseElementChanged: {
		if (baseElement){
			console.log("onHasDateFilterChanged__1")
			if (hasDateFilter){
				baseElement.registerFieldFilterDelegate("DateFilter", timeDelegateFilterComp)
			}
			else {
				console.log("onHasDateFilterChanged__2")
				baseElement.removeFieldFilterDelegate("DateFilter")

			}
		}
	}
	
	onDocumentFilterChanged: {
		if (!baseElement){
			return
		}

		if (documentFilter && hasDocumentFilter){
			baseElement.registerFieldFilterDelegate("DocumentState", stateDelegateFilterComp)
		}
		else{
			baseElement.removeFieldFilterDelegate("DocumentState")
		}
	}
	
	LocalizationEvent {
		onLocalizationChanged: {
		}
	}
	
	Connections {
		target: filterPanelDecorator.complexFilter
		function onFilterChanged(){
			clearAllButton.enabled = !filterPanelDecorator.complexFilter.isEmpty()
		}

		function onCleared(){
			tfc.clear()
		}
	}

	TimeFilter {
		id: timeFilter;
		
		function clear(){
			removeTimeRange()
			removeTimeUnit()
			removeInterpretationMode()
			removeUnitMultiplier()
		}
	}

	Button {
		id: leftButton
		z: customFiltersFlickable.z + 1
		width: visible ? height : 0
		height: visible ? Style.controlHeightM : 0
		anchors.left: customFiltersFlickable.left
		
		visible: customFiltersFlickable.visible && customFiltersFlickable.contentWidth > customFiltersFlickable.width
		enabled: customFiltersFlickable.contentX > 0
		
		icon.source: enabled ? "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal)
							 : "../../../" + Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled)
		
		onClicked: {
			customFiltersFlickable.contentX = Math.max(0, customFiltersFlickable.contentX - 100)
		}
		decorator: Component {
			ButtonDecorator {
				radius: width
			}
		}
	}
	
	Flickable {
		id: customFiltersFlickable
		anchors.left: parent.left
		anchors.leftMargin: Style.marginM
		anchors.right: mainFilters.left
		anchors.rightMargin: Style.marginM

		height: Style.controlHeightM
		
		boundsBehavior: Flickable.StopAtBounds
		
		contentWidth: content.width + 2*rightButton.width
		contentHeight: content.height
		clip: true
		visible: rightButton.x > (leftButton.x + leftButton.width)
		
		Row {
			id: content
			height: Style.controlHeightM
			spacing: Style.marginM
		
			Row {
				height: content.height
				spacing: Style.marginM
				
				Item {
					width: visible ? leftButton.width : 0
					height: visible ? width : 0
					visible: leftButton.visible
				}
				
				Repeater {
					model: filterPanelDecorator.baseElement ? filterPanelDecorator.baseElement.filtersModel : 0
					delegate: Loader {
						id: delegateLoader
						sourceComponent: model.comp
						onLoaded: {
							item.collectionFilter = filterPanelDecorator.complexFilter
							item.documentFilter = filterPanelDecorator.documentFilter
						}
						
						Connections {
							target: filterPanelDecorator
							function onComplexFilterChanged(){
								if (delegateLoader.item){
									delegateLoader.item.collectionFilter = filterPanelDecorator.complexFilter
								}
							}
							
							function onDocumentFilterChanged(){
								if (delegateLoader.item){
									delegateLoader.item.documentFilter = filterPanelDecorator.documentFilter
								}
							}
						}
					}
				}
			}
		}
	}
	
	Button {
		id: rightButton
		z: customFiltersFlickable.z + 1
		width: height
		height: Style.controlHeightM
		anchors.right: customFiltersFlickable.right
		anchors.rightMargin: Style.marginM

		visible: customFiltersFlickable.visible && customFiltersFlickable.contentWidth > customFiltersFlickable.width
		enabled: customFiltersFlickable.contentX < (customFiltersFlickable.contentWidth - customFiltersFlickable.width)

		icon.source: enabled ? "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal)
							 : "../../../" + Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled)

		onClicked: {
			let maxScroll = customFiltersFlickable.contentWidth - customFiltersFlickable.width
			customFiltersFlickable.contentX = Math.min(maxScroll, customFiltersFlickable.contentX + 100)
		}
		
		decorator: Component {
			ButtonDecorator {
				radius: width
			}
		}
	}
	
	Component {
		id: stateDelegateFilterComp
		OptionsFilterDelegate {
			id: stateDelegateFilter
			name: qsTr("Document State")

			Component.onCompleted: {
				createAndAddOption("Active", qsTr("Active"), "", true)
				createAndAddOption("Disabled", qsTr("Disabled"), "", true)
			}
			
			onOptionSelectionChanged: {
				if (!documentFilter){
					return
				}
				
				documentFilter.clear()
				
				if (optionIndexes.length > 0){
					let optionIndex = optionIndexes[0]
					if (optionIndex === 0){
						documentFilter.addDocumentState(documentFilter.s_activeState)
					}
					else{
						documentFilter.addDocumentState(documentFilter.s_disabledState)
					}
				}
				
				documentFilter.filterChanged()
			}
		}
	}
	
	Component {
		id: timeDelegateFilterComp
		
		FilterDelegateBase {
			id: timeDelegateFilter
			name: qsTr("Date")
			onClearFilter: {
				timeDelegateFilter.mainButtonText = name
				timeDelegateFilter.isActive = false;
				timeFilter.clear();
				
				if (filterPanelDecorator.complexFilter){
					filterPanelDecorator.complexFilter.clearTimeFilter();
					filterPanelDecorator.complexFilter.filterChanged()
				}
			}
			
			onOpenFilter: {
				var point = mapToItem(null, x, y + height);
				ModalDialogManager.openDialog(timeFilterParamComp, {"x": point.x, "y": point.y, "timeFilter": timeFilter});
			}
			
			Component {
				id: timeFilterParamComp;
				
				TimeFilterParamView {
					onAccepted: {
						if (filterPanelDecorator.complexFilter){
							filterPanelDecorator.complexFilter.setTimeFilter(this.timeFilter);
							filterPanelDecorator.complexFilter.filterChanged()
						}
						
						ModalDialogManager.closeByComp(timeFilterParamComp);
						timeDelegateFilter.isActive = true;
						
						timeDelegateFilter.mainButtonText = name
					}
					
					onCancelled: {
						timeDelegateFilter.clearFilter()
					}
				}
			}
		}
	}

	Row {
		id: mainFilters
		anchors.right: clearAllButton.left
		anchors.rightMargin: Style.marginM
		height: Style.controlHeightM
		spacing: Style.marginM
		
		SearchTextInput {
			id: tfc;

			onSearchChanged: {
				if (filterPanelDecorator.complexFilter){
					filterPanelDecorator.complexFilter.setTextFilter(tfc.text);
					filterPanelDecorator.complexFilter.filterChanged()
				}
			}
		}
	}
	
	ToolButton {
		id: clearAllButton
		anchors.right: parent.right
		icon.source: enabled ? "qrc:/" + Style.getIconPath("Icons/FilterRemove", Icon.State.On, Icon.Mode.Normal)
							 : "qrc:/" + Style.getIconPath("Icons/FilterRemove", Icon.State.Off, Icon.Mode.Disabled)
		width: Style.buttonWidthL
		height: width
		enabled: false
		tooltipText: qsTr("Reset all filters")
		visible: filterPanelDecorator.canResetFilters
		onClicked: {
			if (filterPanelDecorator.complexFilter){
				filterPanelDecorator.complexFilter.clearAllFilters()
				filterPanelDecorator.complexFilter.filterChanged()
			}
		}
		decorator: Component {
			ToolButtonDecorator {
				icon.width: 26
			}
		}
	}
}

