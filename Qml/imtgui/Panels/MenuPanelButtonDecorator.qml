import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

// One entry of the navigation rail: icon on the left, label beside it.
//
// It used to be a tile with the label centred under the icon, which left the
// text about eighty pixels to live in - anything longer than a single short
// word was cut to an ellipsis. Laid out along the row instead, a name has the
// whole width of the panel, and the panel can be pulled in to the icons without
// any of them moving: the icon sits a fixed margin from the left edge in both
// states, and only the label goes.
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

	// Named to the panel, which owns the one card the whole rail shares: a card
	// per row would mean as many idle items as there are pages.
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

	// Marks the page you are on down the edge of the rail, where it stays
	// legible after the panel is pulled in to the icons.
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

	// The row keeps the open width and is clipped, so the pill has to be told
	// where the rail's edge is - it is the one part that follows the slide, and
	// a rectangle is cheap to resize where re-eliding a label is not.
	Rectangle {
		id: marker;

		anchors.left: parent.left;
		anchors.leftMargin: Style.marginS;
		anchors.top: parent.top;
		anchors.topMargin: 0;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: 0;

		width: (!leftPanelElement.panel ? parent.width : leftPanelElement.panel.width) - 2 * Style.marginS;

		radius: Style.buttonRadius;
		color: leftPanelElement.isSelected ? Style.selectedColor
			: leftPanelElement.isHighlighted ? Style.backgroundColor2 : "transparent";
	}

	Image {
		id: image;

		anchors.left: parent.left;
		// The same distance from the edge whether the panel is open or shut, so
		// collapsing reads as the labels leaving rather than everything moving.
		anchors.leftMargin: Style.marginL;
		anchors.verticalCenter: parent.verticalCenter;

		width: leftPanelElement.iconSize;
		height: width;
		fillMode: Image.PreserveAspectFit;
		sourceSize.width: width;
		sourceSize.height: height;
		source: leftPanelElement.baseElement && leftPanelElement.baseElement.iconSource ? leftPanelElement.baseElement.iconSource : "";
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
		color: leftPanelElement.isSelected || leftPanelElement.isHighlighted ? Style.iconColorOnSelected : Style.textColor;
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
