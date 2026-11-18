import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

// Title of a group of pages in the menu panel. A click folds the group.
// In the collapsed (icon only) panel the title turns into a thin separator;
// with separatorOnly it is just the separator that closes a group.
Item {
	id: groupHeader;

	objectName: groupHeader.groupId + "GroupHeader";

	property string groupId: "";
	property string title: "";
	property bool groupCollapsed: false;
	property bool panelCollapsed: false;
	property bool separatorOnly: false;

	// Width of the folded (icon only) panel: the separator stays inside it.
	property real collapsedPanelWidth: 0;

	readonly property bool showTitle: !groupHeader.panelCollapsed && !groupHeader.separatorOnly;

	signal clicked();

	height: groupHeader.showTitle ? Style.controlHeightM : Style.marginL;

	Rectangle {
		id: separator;

		anchors.left: parent.left;
		anchors.leftMargin: groupHeader.panelCollapsed ? Style.marginM : Style.marginL;
		anchors.verticalCenter: parent.verticalCenter;

		width: groupHeader.panelCollapsed ? groupHeader.collapsedPanelWidth - 2 * Style.marginM : groupHeader.width - 2 * Style.marginL;

		height: 1;
		visible: !groupHeader.showTitle;
		color: Style.borderColor;
	}

	Text {
		id: titleText;

		anchors.left: parent.left;
		anchors.leftMargin: Style.marginL;
		anchors.right: arrowIcon.left;
		anchors.rightMargin: Style.marginXS;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.marginXS;

		visible: groupHeader.showTitle;
		color: groupHeader.groupCollapsed || ma.containsMouse ? Style.textColor : Style.inactiveTextColor;
		font.family: Style.fontFamilyBold;
		font.pixelSize: Style.fontSizeXS;
		font.capitalization: Font.AllUppercase;
		text: groupHeader.title;
		elide: Text.ElideRight;
	}

	Image {
		id: arrowIcon;

		anchors.right: parent.right;
		anchors.rightMargin: Style.marginL;
		anchors.verticalCenter: titleText.verticalCenter;

		width: Style.iconSizeXS;
		height: width;
		sourceSize.width: width;
		sourceSize.height: height;
		visible: groupHeader.showTitle;
		source: groupHeader.groupCollapsed ? "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal)
										   : "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);
	}

	MouseArea {
		id: ma;

		anchors.fill: parent;

		enabled: groupHeader.showTitle;
		hoverEnabled: true;
		cursorShape: Qt.PointingHandCursor;

		onClicked: {
			groupHeader.clicked();
		}
	}
}
