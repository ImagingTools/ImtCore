import QtQuick 2.12
import Acf 1.0

/**
	Full-area loading overlay for collection/document views.

	VS Code style: a thin spinning ring (accent arc over a faint track) with a quiet
	"Loading..." caption under it in larger areas. The ring is painted once and only
	rotated (Timer-driven, no Behavior), so the animation costs no repaints.
*/
Rectangle {
	id: root
	// Test instrumentation: same objectName as BusyIndicator.qml so GUI tests can wait for
	// "whichever busy indicator is currently showing". Inert - no runtime/visual effect.
	objectName: "BusyIndicator"

	// Small sizes (inline indicators) give a ring of that size and no caption.
	property int indicatorSize: 44
	property color accentColor: Style.buttonPrimaryColor
	property string text: qsTr("Loading...")
	property alias background: backgroundRec
	// Rotation of the ring in degrees
	property int animPhase: 0
	readonly property int ringSize: root.indicatorSize <= 30 ? root.indicatorSize : Style.iconSizeM
	readonly property bool captionVisible: root.indicatorSize > 30 && root.height >= Style.sizeHintBXS
	color: "transparent"

	onAccentColorChanged: {
		ring.requestPaint()
	}

	function start(){
		root.animPhase = 0
		root.visible = true
	}

	function stop(){
		root.visible = false
	}

	Rectangle {
		id: backgroundRec

		anchors.fill: parent
		color: Style.baseColor
	}

	Column {
		z: root.z + 1
		anchors.centerIn: parent
		spacing: Style.marginS

		Canvas {
			id: ring

			anchors.horizontalCenter: parent.horizontalCenter
			width: root.ringSize
			height: width
			rotation: root.animPhase

			property string trackColor: Style.borderColor

			onTrackColorChanged: {
				ring.requestPaint()
			}

			onWidthChanged: {
				ring.requestPaint()
			}

			onPaint: {
				let ctx = ring.getContext("2d")
				ctx.clearRect(0, 0, ring.width, ring.height)
				let lineWidth = Math.max(2, Math.round(ring.width / 12))
				let r = ring.width / 2 - lineWidth
				ctx.lineWidth = lineWidth
				ctx.lineCap = "round"

				ctx.strokeStyle = ring.trackColor
				ctx.beginPath()
				ctx.arc(ring.width / 2, ring.height / 2, r, 0, 2 * Math.PI, false)
				ctx.stroke()

				ctx.strokeStyle = String(root.accentColor)
				ctx.beginPath()
				ctx.arc(ring.width / 2, ring.height / 2, r, -Math.PI / 2, Math.PI / 4, false)
				ctx.stroke()
			}
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			visible: root.captionVisible
			text: root.text
			color: Style.subtitleColor
			font.family: Style.fontFamily
			font.pixelSize: Style.fontSizeS
		}
	}

	Timer {
		interval: 30
		running: root.visible
		repeat: true

		onTriggered: {
			root.animPhase = (root.animPhase + 12) % 360
		}
	}

	// Block interaction with the content under the overlay.
	MouseArea {
		anchors.fill: parent
		hoverEnabled: true
		preventStealing: true
		acceptedButtons: Qt.AllButtons

		onWheel: {
			wheel.accepted = true
		}
		onClicked: {
			mouse.accepted = true
		}
		onReleased: {}
		onPressAndHold: {}
		onPressed: {}
		onPositionChanged: {}
	}
}
