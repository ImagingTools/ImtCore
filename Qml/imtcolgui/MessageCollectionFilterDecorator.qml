import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0

DecoratorBase {
	id: mainItem;
	
	width: baseElement ? baseElement.width: 0;
	height: Style.controlHeightL;
	
	property alias segmentedButton: segmentedButton_
	property alias filtermenu: filtermenu_
	property CollectionFilter complexFilter: baseElement ? baseElement.complexFilter : null;	
	
	Component.onCompleted: {
		checkWidth();
	}

	onWidthChanged: {
		checkWidth();
	}
	
	function checkWidth(){
		if (width - filtermenu.width <= segmentedButton.width + 2 * segmentedButton.spacing){
			segmentedButton.visible = false;
		}
		else{
			segmentedButton.visible = true;
		}
	}
	
	FieldFilter {
		id: categoryFilter
		m_fieldId: "Category"
		m_filterValueType: "Integer"
		m_filterOperations: ["Equal"]
	}
	
	GroupFilter {
		id: groupFilter
		m_logicalOperation: "Or"
	}
	
	function updateFilter(){
		if (!groupFilter.hasFieldFilters()){
			groupFilter.createFieldFilters();
		}

		groupFilter.m_fieldFilters.clear()
		
		complexFilter.removeGroupFilter(groupFilter)
		
		if (infoFilter.checked){
			let filter = categoryFilter.copyMe();
			filter.m_filterValue = "1"
			groupFilter.m_fieldFilters.addElement(filter)
		}
		
		if (warningFilter.checked){
			let filter = categoryFilter.copyMe();
			filter.m_filterValue = "2"
			groupFilter.m_fieldFilters.addElement(filter)
		}
		
		if (errorFilter.checked){
			let filter = categoryFilter.copyMe();
			filter.m_filterValue = "3"
			groupFilter.m_fieldFilters.addElement(filter)
		}
		
		if (criticalFilter.checked){
			let filter = categoryFilter.copyMe();
			filter.m_filterValue = "4"
			groupFilter.m_fieldFilters.addElement(filter)
		}
		
		if (verboseFilter.checked){
			let filter = categoryFilter.copyMe();
			filter.m_filterValue = "0"
			groupFilter.m_fieldFilters.addElement(filter)
		}
		
		complexFilter.addGroupFilter(groupFilter)
		
		complexFilter.filterChanged()
	}
	
	SegmentedButton {
		id: segmentedButton_
		anchors.left: parent.left;
		anchors.verticalCenter: parent.verticalCenter;
		
		height: parent.height
		
		Button {
			id: infoFilter;
			
			anchors.verticalCenter: parent.verticalCenter;
			checkable: true
			checked: true
			text: qsTr("Info")
			iconSource: "../../../../" + Style.getIconPath("Icons/Info", Icon.State.On, Icon.Mode.Normal);
			widthFromDecorator: true;
			heightFromDecorator: true;
			onCheckedChanged: {
				mainItem.updateFilter()
			}
		}
		
		Button {
			id: warningFilter;
			
			anchors.verticalCenter: parent.verticalCenter;
			checkable: true
			checked: true
			text: qsTr("Warning")
			iconSource: "../../../../" + Style.getIconPath("Icons/Warning", Icon.State.On, Icon.Mode.Normal);
			widthFromDecorator: true;
			heightFromDecorator: true;
			onCheckedChanged: {
				mainItem.updateFilter()
			}
		}
		
		Button {
			id: errorFilter;
			
			anchors.verticalCenter: parent.verticalCenter;
			checkable: true
			checked: true
			text: qsTr("Error");
			iconSource: "../../../../" + Style.getIconPath("Icons/Error", Icon.State.On, Icon.Mode.Normal);
			widthFromDecorator: true;
			heightFromDecorator: true;
			onCheckedChanged: {
				mainItem.updateFilter()
			}
		}
		
		Button {
			id: criticalFilter;
			
			anchors.verticalCenter: parent.verticalCenter;
			checkable: true
			checked: true
			text: qsTr("Critical");
			iconSource: "../../../../" + Style.getIconPath("Icons/Critical", Icon.State.On, Icon.Mode.Normal);
			widthFromDecorator: true;
			heightFromDecorator: true;
			onCheckedChanged: {
				mainItem.updateFilter()
			}
		}
		
		Button {
			id: verboseFilter;
			
			anchors.verticalCenter: parent.verticalCenter;
			checkable: true
			checked: false
			iconSource: "../../../../" + Style.getIconPath("Icons/Diagnostics", Icon.State.On, Icon.Mode.Normal);
			widthFromDecorator: true;
			heightFromDecorator: true;
			text: qsTr("Verbose");
			onCheckedChanged: {
				mainItem.updateFilter()
			}
		}
		
	}
	
	FilterPanelDecorator {
		id: filtermenu_
		anchors.verticalCenter: parent.verticalCenter;
		anchors.right: parent.right;
		
		baseElement: mainItem.baseElement;
		complexFilter: mainItem.complexFilter;
		width: 325;
	}
}

