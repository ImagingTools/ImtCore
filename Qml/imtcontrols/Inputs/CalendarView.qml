import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ListView{
	id: calendarView;

	clip: true;

	snapMode: ListView.SnapOneItem;
	orientation: ListView.Horizontal;
	boundsBehavior: Flickable.StopAtBounds;
	flickableDirection: Flickable.HorizontalFlick;

	property bool canFillModel: false;
	property bool canSetIndexes: true;
	property real movementStartX: 0;

	property Component weekDayCellDelegate: weekDayCellComp;
	property Component dayCellDelegate: dayCellComp;

	property int radius: Style.radiusL;
	property int weekDayListHeight: Style.headerHeight;

	property ListModel dayOfWeekModel: ListModel{};

	property string color: "#ffffff";
	property string cellColor: "#ffffff";
	property string fontColor_cell: "#000000";
	property int fontSize_cell: 12;

	property int gridCellWidth: Math.trunc(width/7);
	property int gridCellHeight: Math.trunc((height - weekDayListHeight)/6);

	property var dayOfWeek:
		[
		qsTr("Mo"),
		qsTr("Tu"),
		qsTr("We"),
		qsTr("Th"),
		qsTr("Fr"),
		qsTr("Sa"),
		qsTr("Su"),
	];

	Component.onCompleted: {
		if(!dayOfWeekModel.count){
			for(let i = 0; i < dayOfWeek.length; i++){
				let name = dayOfWeek[i];
				dayOfWeekModel.append({"name": name});
			}
		}
	}

	delegate: Rectangle{
		id: monthRec;

		width: calendarView.width;
		height: calendarView.height;
		radius: calendarView.radius;
		color: calendarView.color;

		Row{
			id: dayOfWeekList;

			anchors.top: parent.top;
			anchors.left: parent.left;
			anchors.right: parent.right;

			height: calendarView.weekDayListHeight;
			spacing: 0;
			Repeater {
				id: dayOfWeekRepeater;

				model: calendarView.dayOfWeekModel;
				delegate: calendarView.weekDayCellDelegate;
			}
		}

		GridView{
			id: grid;

			anchors.top: dayOfWeekList.bottom;
			anchors.bottom: parent.bottom;

			width: parent.width;
			cellWidth: calendarView.gridCellWidth;
			cellHeight: calendarView.gridCellHeight;
			boundsBehavior: Flickable.StopAtBounds;

			model: modelData.DayModel;
			delegate: calendarView.dayCellDelegate;
		}//grid
	}


	//Components;

	Component{
		id: weekDayCellComp;

		Rectangle{

			width: calendarView.gridCellWidth;
			height: calendarView.weekDayListHeight;

			color: calendarView.cellColor;

			Text{
				anchors.centerIn: parent;

				font.family: Style.fontFamily;
				font.pixelSize: calendarView.fontSize_cell;
				font.bold: true;
				color: calendarView.fontColor_cell;

				text: model.name;
			}
		}
	}

	Component{
		id: dayCellComp;

		Rectangle{
			width: calendarView.gridCellWidth;
			height: calendarView.gridCellHeight;

			color: calendarView.cellColor;

			Text{
				anchors.centerIn: parent;

				font.family: Style.fontFamily;
				font.pixelSize: calendarView.fontSize_cell;
				color: calendarView.fontColor_cell;

				opacity: modelData.CurrMonth ? 1: 0.5;

				text: modelData.Day;
			}
		}

	}
}
