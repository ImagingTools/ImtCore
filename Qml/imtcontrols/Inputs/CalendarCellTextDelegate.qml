import QtQuick 2.12

import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

CalendarCellDelegateBase {
	id: dayDelegate;

	Text{
		anchors.centerIn: parent;

		font.family: Style.fontFamily;
		font.pixelSize: dayDelegate.calendarItem.fontSize_cell;
		color: dayDelegate.calendarItem.fontColor_cell;
		opacity: modelData.CurrMonth ? 1: 0.5;

		text: modelData.Day;
	}

}
