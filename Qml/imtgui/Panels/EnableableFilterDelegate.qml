import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0

Item {
	id: filterDelegate

	width: switchControl.width
	implicitWidth: switchControl.width
	height: Style.controlHeightM
	objectName: filterId

	property string filterId
	property string name
	property bool isActive: switchControl.checked
	property bool readOnly: false
	property FilterMenu filterMenu: null
	property CollectionFilter collectionFilter: null

	property FieldFilter defaultFieldFilter: FieldFilter {
		m_filterValueType: "String"
		m_filterOperations: ["Equal"]
	}
	property string filterValue: ""
	property bool allowEmptyFilterValue: false

	// The same face to FilterPanelDecorator as FilterDelegateBase. Folded into
	// the overflow menu, a switch is a checkable row that flips it.
	readonly property bool toggleable: true
	readonly property string valueText: ""
	property Item popupAnchorItem: null

	signal clearFilter(bool beQuiet)
	signal openFilter()

	function optionCount(){
		return 0
	}

	onOpenFilter: {
		if (!filterDelegate.readOnly){
			switchControl.setChecked(!switchControl.checked)
		}
	}

	Connections {
		target: filterDelegate.filterMenu ? filterDelegate.filterMenu : null
		function onClearAllFilters(beQuiet){
			filterDelegate.clearFilter(beQuiet)
		}
	}

	QtObject {
		id: internal
		property bool beQuiet: false
	}

	onClearFilter: {
		if (switchControl.checked){
			internal.beQuiet = beQuiet
			switchControl.setChecked(false)
			internal.beQuiet = false
		}
	}

	SwitchCustom {
		id: switchControl
		text: filterDelegate.name
		anchors.verticalCenter: parent.verticalCenter
		readOnly: filterDelegate.readOnly

		onCheckedChanged: {
			if (!filterDelegate.collectionFilter){
				return
			}

			if (checked){
				if (!filterDelegate.allowEmptyFilterValue && filterDelegate.filterValue === ""){
					return
				}
				var filter = filterDelegate.defaultFieldFilter.copyMe()
				filter.m_filterValue = filterDelegate.filterValue
				filterDelegate.collectionFilter.addFieldFilter(filter)
			}
			else{
				filterDelegate.collectionFilter.removeFilterByFieldId(filterDelegate.defaultFieldFilter.m_fieldId)
			}

			if (!internal.beQuiet){
				filterDelegate.collectionFilter.filterChanged()
			}
		}
	}
}
