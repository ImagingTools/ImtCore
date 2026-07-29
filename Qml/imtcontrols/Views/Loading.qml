import QtQuick 2.12
import Acf 1.0

/**
	Full-area loading overlay for collection/document views.

	Surface: Style.baseColor (matches Agents/Services/Messages tables).
	Indicator: three accent dots with a staggered opacity cycle (no Canvas,
	no Behavior). Compact enough for tiny inline spinners too.
*/
Rectangle {
	id: root
	// Test instrumentation: same objectName as BusyIndicator.qml (a separate, parallel spinner
	// component) so GUI tests can wait for "whichever busy indicator is currently showing" without
	// needing to know which of the two a given view uses. Its rotation animation (a Timer mutating
	// `rotation` every 10ms) never touches objectName/visible/childList, so a MutationObserver-based
	// "DOM went quiet" wait can resolve while it's still visibly spinning. Inert - no runtime/visual
	// effect.
	objectName: "BusyIndicator"

	property int indicatorSize: 44
	property color accentColor: Style.textSelectedColor
	property alias background: backgroundRec
	// 0..2 — which dot is currently brightest
	property int animPhase: 0
	color: "transparent"

	function start(){
		root.animPhase = 0
		root.visible = root.width >= dotsRow.width || root.indicatorSize <= 20
	}

	function stop(){
		root.visible = false
	}

	Rectangle {
		id: backgroundRec

		anchors.fill: parent
		color: Style.baseColor
	}

	Row {
		id: dotsRow

		z: root.z + 1
		anchors.centerIn: parent
		spacing: Math.max(3, Math.round(root.indicatorSize * 0.16))
		visible: root.width >= width || root.indicatorSize <= 20

		Repeater {
			model: 3

			delegate: Rectangle {
				// Fixed size so the row width never jumps while animating.
				width: Math.max(4, Math.round(root.indicatorSize * 0.22))
				height: width
				radius: width / 2
				color: root.accentColor
				// Active dot full opacity; neighbors soft.
				opacity: ((root.animPhase - index + 3) % 3) === 0
						? 1
						: (((root.animPhase - index + 3) % 3) === 1 ? 0.45 : 0.2)
			}
		}
	}

	Timer {
		interval: 260
		running: root.visible
		repeat: true

		onTriggered: {
			root.animPhase = (root.animPhase + 1) % 3
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
