import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtauthgui 1.0

Rectangle {
	id: historyPanel;
	z: parent.z + 1;
	anchors.top: parent.top;
	anchors.right: parent.right;
	anchors.bottom: parent.bottom;

	width: contentWidth;
	color: Style.baseColor;

	property bool opened: true;
	property int contentWidth: 500;
	property alias contentHeight: historyView.contentHeight;
	property string documentId: "";
	property alias collectionId: historyView.collectionId;
	property Flickable editorFlickable;

	Component.onCompleted: {
		historyPanel.visible = PermissionsController.checkPermission("ViewRevisions");
	}

	Connections {
		target: historyPanel.editorFlickable;

		function onContentXChanged(){
			historyView.flickable.contentX = historyPanel.editorFlickable.contentX;
		}

		function onContentYChanged(){
			historyView.flickable.contentY = historyPanel.editorFlickable.contentY;
		}
	}

	NumberAnimation {
		id: animation;
		property: "width";
		duration: 200;
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
		anchors.left: parent.left;
		anchors.leftMargin: Style.sizeLargeMargin;
		anchors.top: parent.top;
		anchors.topMargin: Style.sizeLargeMargin;
		anchors.right: parent.right;
		anchors.rightMargin: Style.sizeLargeMargin;
		title: qsTr("History");

		controlComp: Component {
			id: baseControlComp;

			ToolButton {
				id: openButton;

				height: 22;
				width: height;
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
		anchors.left: parent.left;
		anchors.leftMargin: Style.sizeLargeMargin;
		anchors.top: historyHeader.bottom;
		anchors.bottom: parent.bottom;
		anchors.right: parent.right;
		anchors.rightMargin: Style.sizeLargeMargin;
		visible: historyPanel.width !== 0;
		documentId: historyPanel.documentId;
	}
}

