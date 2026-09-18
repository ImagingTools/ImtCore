import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

DecoratorBase {
	id: leftPanelElement;

	width: Style.sizeHintXXS;
	height: Style.controlHeightL;

	signal accepted(string text);
	signal clicked();

	readonly property Item panel: !leftPanelElement.baseElement ? null : leftPanelElement.baseElement.menuPanelRef;
	readonly property bool collapsed: !leftPanelElement.panel ? false : leftPanelElement.panel.collapsed;
	readonly property bool isSelected: !leftPanelElement.baseElement ? false : leftPanelElement.baseElement.selected;
	readonly property bool isHighlighted: !leftPanelElement.baseElement ? false : leftPanelElement.baseElement.highlighted;
	readonly property string title: !leftPanelElement.baseElement ? "" : leftPanelElement.baseElement.text;
	readonly property int iconSize: !leftPanelElement.panel ? Style.menuPanelIconSize : leftPanelElement.panel.iconSize;

	property bool textIsCropped: helperText.width > description.width;

	onWidthChanged: {
		if(leftPanelElement.baseElement){
			leftPanelElement.baseElement.contentWidth = width;
		}
	}

	onHeightChanged: {
		if(leftPanelElement.baseElement){
			leftPanelElement.baseElement.contentHeight = height;
		}
	}

	onCollapsedChanged: leftPanelElement.updateHint();
	onIsHighlightedChanged: leftPanelElement.updateHint();

	function updateHint(){
		if (!leftPanelElement.panel){
			return;
		}

		if (leftPanelElement.collapsed && leftPanelElement.isHighlighted){
			leftPanelElement.panel.showHint(leftPanelElement.title,
				leftPanelElement.mapToItem(leftPanelElement.panel, 0, leftPanelElement.height / 2).y);
		}
		else{
			leftPanelElement.panel.hideHint(leftPanelElement.title);
		}
	}

	Rectangle {
		id: activeBar;

		anchors.left: parent.left;
		anchors.verticalCenter: parent.verticalCenter;

		width: Style.spacingXS;
		height: parent.height - 2 * Style.spacingXS;
		radius: width / 2;
		visible: leftPanelElement.isSelected;
		color: Style.iconColorOnSelected;
	}

	Rectangle {
		id: marker;

		anchors.left: parent.left;
		anchors.leftMargin: Style.marginS;
		anchors.top: parent.top;
		anchors.topMargin: 0;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: 0;

		width: (!leftPanelElement.panel ? parent.width : leftPanelElement.panel.width) - 2 * Style.marginS;

		radius: Style.marginS;
		color: leftPanelElement.isSelected ? Style.selectedColor
			: leftPanelElement.isHighlighted ? Style.alternateBaseColor : "transparent";
	}

	Image {
		id: image;

		anchors.left: parent.left;
		anchors.leftMargin: Style.marginL;
		anchors.verticalCenter: parent.verticalCenter;

		width: leftPanelElement.iconSize;
		height: width;
		fillMode: Image.PreserveAspectFit;
		sourceSize.width: width;
		sourceSize.height: height;
		source: leftPanelElement.baseElement && leftPanelElement.baseElement.iconSource ? leftPanelElement.baseElement.iconSource : "";
		// Full strength where the pointer is or where you already are, held back
		// everywhere else - the same weighting the tabs use.
		opacity: leftPanelElement.isSelected || leftPanelElement.isHighlighted ? 1.0 : Style.opacityHigh;
	}

	Text {
		id: description;

		anchors.left: image.right;
		anchors.leftMargin: Style.marginM;
		anchors.right: parent.right;
		anchors.rightMargin: Style.marginL;
		anchors.verticalCenter: parent.verticalCenter;

		// Switched, not faded: a fade on every row is work on every frame of the
		// slide, and the label has nowhere useful to be halfway through one.
		visible: !leftPanelElement.collapsed;
		// Selection recolours the label; hover only lifts the row behind it. The
		// two used to share a colour, so passing the pointer over a row made it
		// look picked.
		color: leftPanelElement.isSelected ? Style.titleColor : Style.textColor;
		// The page you are on carries the weight, so the rail says where you are
		// without relying on the tint alone.
		font.family: leftPanelElement.isSelected ? Style.fontFamilyBold : Style.fontFamily;
		font.pixelSize: Style.fontSizeM;
		text: leftPanelElement.title;
		verticalAlignment: Text.AlignVCenter;
		elide: Text.ElideRight;
	}

	Text {
		id: helperText;

		font.family: description.font.family;
		font.pixelSize: description.font.pixelSize;
		text: description.text;
		wrapMode: Text.NoWrap;
		visible: false;
	}

	// Open, a name that had to be cut is still worth reading in full.
	TooltipArea {
		anchors.fill: parent;

		mouseArea: !leftPanelElement.baseElement ? null : leftPanelElement.baseElement.mouseArea;
		text: !leftPanelElement.collapsed && leftPanelElement.textIsCropped ? leftPanelElement.title : "";
	}
}
