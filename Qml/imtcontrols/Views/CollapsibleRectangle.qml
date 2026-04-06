import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Rectangle{
	width: 500
	height: !expandButton.expanded ? minHeight : maxHeight

	clip: true;

	color: Style.baseColor

	border.color: Style.borderColor
	radius: Style.radiusXL

	property int minHeight: 50
	property int maxHeight: 400
	property int buttonMargin: Style.marginL

	property alias iconSource: expandButton.iconSource

	ToolButton{
		id: expandButton
		anchors.top: parent.top;
		anchors.right: parent.right
		anchors.topMargin: parent.buttonMargin
		anchors.rightMargin: parent.buttonMargin

		width: height;
		height: Style.buttonHeightS;

		iconSource: "../../../" + Style.getIconPath(expanded ? "Icons/Up" : "Icons/Down", Icon.State.On, Icon.Mode.Normal);

		tooltipText: expanded ? qsTr("Collapse") : qsTr("Expand");

		property bool expanded: true

		onClicked: {
			expanded = !expanded
		}
	}
}

