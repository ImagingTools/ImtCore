import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import imtbaseComplexCollectionFilterSdl 1.0

PopupView {
	id: root;
	
	width: 260;
	height: content.height + 2 * Style.marginM;
		
	property TimeFilter timeFilter: TimeFilter {}
	property alias listView: listView;
	property alias treeModel: treeModel;
	
	property Component buttonDecorator: ButtonDecorator {
				color: Style.backgroundColor2;
			}

	property int fontSize: Style.fontSizeM;
	property var datePickerParams;
	
	signal accepted(string id, string name);
	signal cancelled();
	
	TreeItemModel {
		id: treeModel;
		Component.onCompleted: {
			root.updateViewModel();
		}
	}
	
	onTimeFilterChanged: {
		updateGui();
	}
	
	function updateGui(){
		if (timeFilter.m_timeRange){
			fromDateItem.visible = true;
			
			let beginDate = timeFilter.m_timeRange.m_begin;
			let fromDate = new Date();
			if (beginDate !== ""){
				fromDate = new Date(beginDate)
			}
			
			fromDatePicker.setDate(fromDate);

			let endDate = timeFilter.m_timeRange.m_end;
			
			let toDate = new Date();
			if (endDate !== ""){
				toDate = new Date(endDate)
			}

			toDatePicker.setDate(toDate);
		}
		
		listView.currentIndex = -1;
		
		let unit = timeFilter.m_timeUnit;
		let mode = timeFilter.m_interpretationMode;
		
		for (let i = 0; i < treeModel.getItemsCount(); i++){
			let id = treeModel.getData("id", i);
			let interpretationMode = treeModel.getData("InterpretationMode", i);
			if (unit == id && interpretationMode == mode){
				listView.currentIndex = i;
				break;
			}
		}
	}
	
	function updateViewModel(){
		treeModel.clear();
		
		let index = treeModel.insertNewItem();
		treeModel.setData("id", "Hour", index);
		treeModel.setData("name", qsTr("Last hour"), index);
		treeModel.setData("InterpretationMode", "For", index);
		
		index = treeModel.insertNewItem();
		treeModel.setData("id", "Day", index);
		treeModel.setData("name", qsTr("Today"), index);
		treeModel.setData("InterpretationMode", "Current", index);
		
		index = treeModel.insertNewItem();
		treeModel.setData("id", "Week", index);
		treeModel.setData("name", qsTr("This week"), index);
		treeModel.setData("InterpretationMode", "Current", index);
		
		index = treeModel.insertNewItem();
		treeModel.setData("id", "Month", index);
		treeModel.setData("name", qsTr("This month"), index);
		treeModel.setData("InterpretationMode", "Current", index);
		
		index = treeModel.insertNewItem();
		treeModel.setData("id", "Month", index);
		treeModel.setData("name", qsTr("Last month"), index);
		treeModel.setData("InterpretationMode", "Last", index);
		
		index = treeModel.insertNewItem();
		treeModel.setData("id", "Year", index);
		treeModel.setData("name", qsTr("This year"), index);
		treeModel.setData("InterpretationMode", "Current", index);
		
		index = treeModel.insertNewItem();
		treeModel.setData("id", "Year", index);
		treeModel.setData("name", qsTr("Last year"), index);
		treeModel.setData("InterpretationMode", "Last", index);
		
		listView.model = treeModel;
	}
	
	Rectangle {
		id: background;
		anchors.fill: parent;
		color: Style.baseColor;
		radius: Style.buttonRadius;
		border.width: 1;
		border.color: Style.borderColor;
	}
	
	MouseArea {
		anchors.fill: background;
		onWheel: {
			wheel.accepted = true;
		}
		onClicked: {
			mouse.accepted = true;
		}
		onReleased: {}
		onPressAndHold: {}
		onPressed: {}
		onPositionChanged: {}
	}
	
	Item {
		id: body;
		anchors.fill: parent;
		anchors.margins: Style.marginM;
		
		Column {
			id: content;
			anchors.verticalCenter: parent.verticalCenter;
			width: parent.width;
			spacing: Style.marginM;
			
			ListView {
				id: listView;
				width: parent.width;
				height: 30 * count;
				boundsBehavior: Flickable.StopAtBounds;
				interactive: false;
				delegate: Component {
					PopupMenuDelegate {
						text: model.name;
						highlighted: listView.currentIndex === model.index;
						selected: mouseArea.containsMouse;
						font.pixelSize: root.fontSize
						onClicked: {
							listView.currentIndex = model.index;
							// root.model.clear();
							
							root.timeFilter.m_timeUnit = model.id;
							root.timeFilter.m_interpretationMode = model.InterpretationMode;

							root.accepted(model.id, model.name);
						}
					}
				}
			}
			
			Rectangle {
				id: separator;
				width: parent.width;
				height: 1;
				color: Style.borderColor;
			}
			
			BaseText {
				text: qsTr("From");
				width: parent.width;
				font.pixelSize: root.fontSize
			}
			
			Item {
				id: selectDateItem;
				width: parent.width;
				height: Style.controlHeightM;
				visible: !fromDateItem.visible;
				
				Button {
					width: parent.width;
					height: parent.height;
					text: qsTr("Select a date");
					onClicked: {
						fromDateItem.visible = true;
						toDatePicker.setCurrentDay();
					}
					
					decorator: root.buttonDecorator;
				}
			}
			
			Item {
				id: fromDateItem;
				width: parent.width;
				height: Style.controlHeightM;
				visible: false;

				DateInput {
					id: fromDatePicker;
					anchors.horizontalCenter: parent.horizontalCenter;
					hasTitle: false
					width: parent.width;
				}
			}
			
			BaseText {
				text: qsTr("To");
				width: parent.width;
				font.pixelSize: root.fontSize
			}
			
			Item {
				width: parent.width;
				height: Style.controlHeightM;
				
				DateInput {
					id: toDatePicker;
					anchors.horizontalCenter: parent.horizontalCenter;
					hasTitle: false
					width: parent.width;
				}
			}
			
			Item {
				width: parent.width;
				height: Style.controlHeightM;
				
				Row {
					anchors.horizontalCenter: parent.horizontalCenter;
					height: Style.controlHeightM;
					spacing: Style.marginM;
					
					Button {
						id: applyButton;
						text: qsTr("Apply");
						enabled: fromDateItem.visible;
						onClicked: {
							root.timeFilter.m_timeRange.m_begin = fromDatePicker.selectedDate.toISOString()
							root.timeFilter.m_timeRange.m_end = toDatePicker.selectedDate.toISOString()
							
							root.accepted("TimeRange", fromDatePicker.getDateAsString() + " -> " + toDatePicker.getDateAsString());
						}
						
						decorator: root.buttonDecorator;
					}
					
					Button {
						id: clearButton;
						text: qsTr("Clear");
						enabled: fromDateItem.visible;
						onClicked: {
							fromDateItem.visible = false;
							fromDatePicker.showCurrentDate();
							toDatePicker.showCurrentDate();
							listView.currentIndex = -1;
							
							root.cancelled();
						}
						
						decorator: root.buttonDecorator;
					}
				}
			}
		}
	}
	
	DropShadow {
		id: dropShadow;
		
		anchors.fill: background;
		
		z: background.z-1
		
		horizontalOffset: 2;
		verticalOffset: 2;
		
		radius: Style.radiusM;
		color: Style.shadowColor;
		
		source: background;
	}
	
	function fromFormat(date){
		let dateStr = date.split(" ")[0];
		let data = dateStr.split(".");
		
		return data[2] + "-" + data[1] + "-" + data[0];
	}
	
	function formatDate(date){
		if (!date){
			return "";
		}
		
		let year = date.getFullYear()
		let month = date.getMonth();
		let day = date.getDate();
		
		let monthStr = String(month + 1)
		if (monthStr.length == 1){
			monthStr = "0" + monthStr;
		}
		
		let dayStr = String(day)
		if (dayStr.length == 1){
			dayStr = "0" + dayStr;
		}
		
		return dayStr + "." + monthStr + "." + year + " " + "00:00:00";
	}
}


