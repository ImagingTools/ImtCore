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
	
	property Component buttonDecorator: ButtonDecorator {
				color: Style.backgroundColor2;
			}

	property int fontSize: Style.fontSizeM;

	property bool canTimeRangeEdit: true

	signal accepted(string id, string name);
	signal cancelled();
	
	onTimeFilterChanged: {
		updateGui();
	}
	
	function updateGui(){
		if (!timeFilter.hasTimeRange()){
			timeFilter.createTimeRange()
		}

		if (timeFilter.m_timeRange){
			fromDateItem.visible = true;
			
			let beginDate = timeFilter.m_timeRange.m_begin;
			let fromDate = new Date();
			if (beginDate && beginDate !== ""){
				fromDate = new Date(beginDate)
			}

			fromDatePicker.setDate(fromDate);

			let endDate = timeFilter.m_timeRange.m_end;
			
			let toDate = new Date();
			if (endDate && endDate !== ""){
				toDate = new Date(endDate)
			}

			toDatePicker.setDate(toDate);
		}
		
		listView.currentIndex = -1;
		
		let unit = timeFilter.m_timeUnit;
		let mode = timeFilter.m_interpretationMode;
		
		for (let i = 0; i < presetModel.count; i++){
			if (unit === presetModel.get(i).unit && mode === presetModel.get(i).mode){
				listView.currentIndex = i;
				break;
			}
		}
	}

	function setItemName(index, name){
		if (index < 0 || index >= presetModel.count){
			return
		}

		presetModel.setProperty(index, "name", name)
	}

	function setItemVisible(index, visible){
		if (index < 0 || index >= presetModel.count){
			return
		}

		presetModel.setProperty(index, "visible", visible)
	}

	function setItemUnit(index, unit){
		if (index < 0 || index >= presetModel.count){
			return
		}

		presetModel.setProperty(index, "unit", unit)
	}

	function setItemMode(index, mode){
		if (index < 0 || index >= presetModel.count){
			return
		}

		presetModel.setProperty(index, "mode", mode)
	}

	function addItem(unit, name, mode, index){
		if (index === undefined){
			index = presetModel.count
		}

		presetModel.insert(index, {id: unit, unit: unit, name: name,mode: mode,visible: true})
	}

	ListModel {
		id: presetModel
		Component.onCompleted: {
			root.addItem("Hour", qsTr("Last hour"), "For")
			root.addItem("Day", qsTr("Today"), "Current")
			root.addItem("Week", qsTr("This week"), "Current")
			root.addItem("Month", qsTr("This month"), "Current")
			root.addItem("Month", qsTr("Last month"), "Last")
			root.addItem("Year", qsTr("This year"), "Current")
			root.addItem("Year", qsTr("Last year"), "Last")
		}
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
				height: contentHeight
				boundsBehavior: Flickable.StopAtBounds;
				interactive: false;
				model: presetModel
				delegate: Component {
					PopupMenuDelegate {
						height: visible ? Style.controlHeightM : 0
						text: model.name;
						highlighted: listView.currentIndex === model.index;
						selected: mouseArea.containsMouse;
						font.pixelSize: root.fontSize
						visible: model.visible
						onClicked: {
							listView.currentIndex = model.index;

							root.timeFilter.m_timeUnit = model.unit;
							root.timeFilter.m_interpretationMode = model.mode;

							root.accepted(model.unit, model.name);
						}
					}
				}
			}
			
			Column {
				width: content.width
				spacing: content.spacing
				visible: root.canTimeRangeEdit
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
								if (!root.timeFilter.hasTimeRange()){
									root.timeFilter.createTimeRange()
								}
	
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


