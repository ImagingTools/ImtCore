import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

Rectangle {
	id: historyPanel;
	z: parent.z + 1;
	anchors.top: parent.top;
	anchors.topMargin: Style.size_largeMargin;
	anchors.right: parent.right;
	anchors.rightMargin: Style.size_largeMargin;
	anchors.bottom: parent.bottom;
	anchors.bottomMargin: Style.size_largeMargin;
	color: Style.backgroundColor2;

	width: contentWidth;

	property bool opened: true;
	property int contentWidth: 500;
	property string documentId: "";

	NumberAnimation {
		id: animation;
		property: "width";
		duration: 300;
		target: historyPanel;
	}

	onOpenedChanged: {
		if (opened){
			animation.from = 0;
			animation.to = contentWidth;
		}
		else{
			animation.from = contentWidth;
			animation.to = 0;
		}

		animation.start();
	}

	Rectangle {
		id: separator;
		anchors.top: parent.top;
		anchors.left: parent.left;
		width: 1;
		height: parent.height;
		color: Style.borderColor;
		visible: historyPanel.opened;
	}

	GroupHeaderView {
		id: historyHeader;
		anchors.left: historyView.left;
		anchors.top: parent.top;
		width: parent.width;
		title: qsTr("History");
		// visible: historyPanel.opened;

		controlComp: Component {
			id: baseControlComp;

			ToolButton {
				id: openButton;

				height: 22;
				width: height;
				tooltipText: historyPanel.opened ? qsTr("Hide history") : qsTr("Show history");
				iconSource: historyPanel.opened
							? "../../../" + Style.getIconPath("Icons/History", Icon.State.On, Icon.Mode.Normal)
							: "../../../" + Style.getIconPath("Icons/History", Icon.State.On, Icon.Mode.Normal);
				visible: true;
				checkable: true;
				checked: historyPanel.opened;
				onClicked: {
					historyPanel.opened = !historyPanel.opened;
				}
			}
		}
	}

	DocumentHistoryView {
		id: historyView;
		anchors.top: historyHeader.bottom;
		anchors.topMargin: Style.size_largeMargin;
		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.bottom: parent.bottom;
		width: parent.width - 2 * Style.size_mainMargin;
		visible: historyPanel.width !== 0;
		documentId: historyPanel.documentId;
	}
}

