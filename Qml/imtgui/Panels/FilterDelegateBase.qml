import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

// One filter, as a chip.
//
// It used to be two Buttons sat a pixel apart, each painting an extra rectangle
// over the gap to fake a shared outline - a seam that showed, and a shape that
// changed width as the clear button came and went.
//
// It also showed only the chosen value once a filter was on, so a row of active
// filters read as a list of bare values with no way of telling which field each
// one belonged to. The field keeps its place now and the value joins it.
Item {
	id: filterDelegateBase

	// Implicit, not explicit. A Loader with a width of its own resizes whatever
	// it holds to that width, so a Loader sized from item.width and an item
	// sized from the Loader deadlocked: the chip grew when its filter was set
	// and the row went on reserving the old width, letting it lie over its
	// neighbour. Sized implicitly, the Loader follows.
	implicitWidth: chip.width
	implicitHeight: Style.controlHeightM
	objectName: filterId

	property string filterId
	property string name
	property string mainButtonText: name
	property bool isActive: false
	property bool readOnly: false
	property FilterMenu filterMenu: null

	// What the filter settled on, when that is something other than its own name.
	readonly property string valueText: filterDelegateBase.isActive
			&& filterDelegateBase.mainButtonText !== filterDelegateBase.name ? filterDelegateBase.mainButtonText : ""

	readonly property bool hovered: chipArea.containsMouse
	// True when the value on the chip had to be cut to fit.
	readonly property bool valueIsCropped: valueLabel.visible && valueMetrics.width > valueLabel.width

	// Stands in for the chip when it has been folded into the overflow menu and
	// has no place on screen of its own to hang a popup from.
	property Item popupAnchorItem: null

	property alias mainButton: chipArea
	property alias clearButton: clearArea

	signal clearFilter(bool beQuiet)
	signal openFilter()

	// Where this filter's own popup belongs. The delegates used to work it out
	// as mapToItem(null, x, y + height), which feeds a position in the parent
	// back in as a local offset - so every popup opened as far to the right of
	// its chip as that chip stood from the start of the row.
	function popupPoint(){
		let source = filterDelegateBase.popupAnchorItem ? filterDelegateBase.popupAnchorItem : filterDelegateBase

		return source.mapToItem(null, 0, source.height)
	}

	// What a filter can offer to a menu that wants to present it inline rather
	// than open its own popup. A filter with no list of its own - a date range,
	// say - reports none and is opened the long way instead.
	function optionCount(){
		return 0
	}

	function optionName(index){
		return ""
	}

	function optionEnabled(index){
		return true
	}

	function optionChecked(index){
		return false
	}

	function selectOption(index){
	}

	Connections {
		target: filterDelegateBase.filterMenu ? filterDelegateBase.filterMenu: null
		function onClearAllFilters(beQuiet){
			filterDelegateBase.clearFilter(beQuiet)
		}
	}

	Rectangle {
		id: chip

		// Sized from the token, not from the delegate around it. Read off the
		// parent, the height fed the width - the clear button is a square of it -
		// so the chip could report a width that was a whole button short of what
		// it would actually be drawn at, and the row let it in only to have it
		// cut off at the edge.
		height: Style.controlHeightM
		width: Style.marginM + labelRow.width
			+ (clearArea.visible ? clearArea.width : Style.marginM)
		radius: height / 2

		color: !filterDelegateBase.isActive ? (chipArea.containsMouse ? Style.hover : "transparent")
			: Style.selectedColor
		border.width: 1
		border.color: filterDelegateBase.isActive ? Style.titleColor : Style.borderColor
		opacity: filterDelegateBase.readOnly ? 0.6 : 1

		Row {
			id: labelRow

			anchors.left: parent.left
			anchors.leftMargin: Style.marginM
			anchors.verticalCenter: parent.verticalCenter

			spacing: Style.spacingXS

			BaseText {
				anchors.verticalCenter: parent.verticalCenter

				text: filterDelegateBase.valueText === "" ? filterDelegateBase.name : filterDelegateBase.name + ":"
				color: filterDelegateBase.isActive ? Style.subtitleColor : Style.textColor
			}

			// Capped and elided: one filter with a long value must not push every
			// other one off the line.
			//
			// The cap is measured on a copy that carries no width of its own.
			// Taken from the label's own implicitWidth it fed itself: an elided
			// Text reports the width it was given, so the value could settle at
			// nothing and the chip would read "Field:" with a blank after it.
			BaseText {
				id: valueLabel

				anchors.verticalCenter: parent.verticalCenter

				visible: filterDelegateBase.valueText !== ""
				width: valueLabel.visible ? Math.min(valueMetrics.width, Style.sizeHintXXXS) : 0
				text: filterDelegateBase.valueText
				font.family: Style.fontFamilyBold
				color: Style.titleColor
				elide: Text.ElideRight
			}

			Image {
				id: chevron

				anchors.verticalCenter: parent.verticalCenter

				visible: !filterDelegateBase.isActive
				width: chevron.visible ? Style.iconSizeXS : 0
				height: Style.iconSizeXS
				sourceSize.width: width
				sourceSize.height: height
				source: "qrc:/" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal)
			}
		}

		// Natural width of the value.
		//
		// A plain Text, and eliding switched off on purpose: BaseText turns
		// ElideRight on for everything it is used for, and a Text that elides
		// without a width of its own reports the width of the elided line rather
		// than of the whole string. So the yardstick came in short - by however
		// much the value was cut - and only ever for a chip that had a value at
		// all, which is why it was the active ones that ended up half under the
		// overflow button.
		Text {
			id: valueMetrics

			visible: false
			elide: Text.ElideNone
			wrapMode: Text.NoWrap
			text: filterDelegateBase.valueText
			font.family: valueLabel.font.family
			font.pixelSize: valueLabel.font.pixelSize
		}

		MouseArea {
			id: chipArea

			anchors.left: parent.left
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.right: clearArea.visible ? clearArea.left : parent.right

			hoverEnabled: true
			enabled: !filterDelegateBase.readOnly
			cursorShape: Qt.PointingHandCursor

			onClicked: {
				filterDelegateBase.openFilter()
			}
		}

		// Inside the same outline rather than beside it, so the chip keeps one
		// shape whether or not the filter is on.
		Item {
			id: clearArea

			objectName: "ClearButton"

			anchors.right: parent.right
			anchors.top: parent.top
			anchors.bottom: parent.bottom

			width: clearArea.visible ? Style.controlHeightM : 0
			visible: filterDelegateBase.isActive

			Rectangle {
				anchors.centerIn: parent

				width: Style.iconSizeM
				height: width
				radius: width / 2
				color: clearMouse.containsMouse ? Style.baseColor : "transparent"
			}

			Image {
				anchors.centerIn: parent

				width: Style.iconSizeXS
				height: width
				sourceSize.width: width
				sourceSize.height: height
				source: filterDelegateBase.readOnly ? "qrc:/" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal)
					: "qrc:/" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
			}

			MouseArea {
				id: clearMouse

				anchors.fill: parent

				hoverEnabled: true
				enabled: !filterDelegateBase.readOnly
				cursorShape: Qt.PointingHandCursor

				onClicked: {
					filterDelegateBase.clearFilter(false)
				}
			}
		}
	}
}
