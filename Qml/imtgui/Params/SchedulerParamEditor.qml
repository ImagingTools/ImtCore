import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: schedulerParamEditor
	
	property SchedulerParam schedulerParam: editorModel
	editorModelComp: Component {
		SchedulerParam {}
	}

	sourceComp: Component {
		Item {
			id: root;
		
			width: schedulerParamEditor.width;
			height: column.height;

			Column {
				id: column;
				width: root.width;
				spacing: Style.sizeMainMargin;
		
				GroupElementView {
					id: group;
					width: parent.width;
		
					ElementView {
						id: timePicker;
						name: qsTr("Start Time")
						controlComp: Component {
							TimeInput {
								property string schedulerStartTime: schedulerParamEditor.schedulerParam ? schedulerParamEditor.schedulerParam.m_startTime : ""
								
								hasTitle: false
								
								onSchedulerStartTimeChanged: {
									if (schedulerStartTime != ""){
										let parts = schedulerStartTime.split(" ")
										if (parts.length == 2){
											let time = parts[1]
											setTime(time)
										}
									}
								}
								
								onTimeChanged: {
									if (!schedulerParamEditor.schedulerParam){
										return
									}
									
									schedulerParamEditor.schedulerParam.m_startTime = "01-01-2000 " + value;
								}
							}
						}
					}
					
					property int schedulerInterval: schedulerParamEditor.schedulerParam ? schedulerParamEditor.schedulerParam.m_interval : -1
					onSchedulerIntervalChanged: {
						if (schedulerInterval > 0){
							comboBox.model = comboBoxModel;
						}
					}
		
					ComboBoxElementView {
						id: comboBox;
						name: qsTr("Backup Interval");
						onCurrentIndexChanged: {
							if (!schedulerParamEditor.schedulerParam){
								return
							}

							if (comboBox.currentIndex >= 0){
								let id = comboBox.model.getData("id", comboBox.currentIndex);
								let secondsInHour = 3600;
		
								let newInterval = -1;
								if (id === "Day"){
									newInterval = 24 * secondsInHour;
								}
								else if (id === "Week"){
									newInterval = 24 * secondsInHour * 7;
								}
								else if (id === "Month"){
									newInterval = 24 * secondsInHour * 30;
								}
		
								if (newInterval !== -1 && schedulerParamEditor.schedulerParam.m_interval !== newInterval){
									schedulerParamEditor.schedulerParam.m_interval = newInterval;
								}
							}
						}
		
						onModelChanged: {
							if (!schedulerParamEditor.schedulerParam){
								return
							}
							
							let seconds = schedulerParamEditor.schedulerParam.m_interval;
							var d = Math.floor(seconds / (3600*24));
		
							if (d === 1){
								comboBox.currentIndex = 0;
							}
							else if (d === 7){
								comboBox.currentIndex = 1;
							}
							else if (d === 30){
								comboBox.currentIndex = 2;
							}
						}
					}
				}
		
				TreeItemModel{
					id: comboBoxModel;
		
					Component.onCompleted: {
						let index = comboBoxModel.insertNewItem();
						comboBoxModel.setData("id", "Day", index)
						comboBoxModel.setData("name", qsTr("Day"), index)
		
						index = comboBoxModel.insertNewItem();
						comboBoxModel.setData("id", "Week", index)
						comboBoxModel.setData("name", qsTr("Week"), index)
		
						index = comboBoxModel.insertNewItem();
						comboBoxModel.setData("id", "Month", index)
						comboBoxModel.setData("name", qsTr("Month"), index)
					}
				}
			}
		}
	}
}

