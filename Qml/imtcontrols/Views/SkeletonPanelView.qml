import QtQuick 2.12
import Acf 1.0

/**
	Skeleton of a card or a chart panel: a title line over one content block.
	Same start()/stop() API as Loading; covers and blocks the panel below it.
*/
Rectangle {
	id: skeletonPanel

	// Same objectName as Loading, so GUI tests waiting for the busy indicator keep working.
	objectName: "BusyIndicator"

	property bool titleVisible: true
	property int phase: 0

	color: Style.baseColor
	visible: false

	function start(){
		skeletonPanel.phase = 0
		skeletonPanel.visible = true
	}

	function stop(){
		skeletonPanel.visible = false
	}

	MouseArea {
		anchors.fill: parent
		hoverEnabled: true
		acceptedButtons: Qt.AllButtons
	}

	Timer {
		interval: 80
		repeat: true
		running: skeletonPanel.visible

		onTriggered: {
			skeletonPanel.phase = (skeletonPanel.phase + 1) % 16
		}
	}

	Column {
		anchors.fill: parent
		anchors.margins: Style.marginXL
		spacing: Style.marginL
		opacity: 0.45 + 0.55 * (skeletonPanel.phase < 8 ? skeletonPanel.phase / 8 : (16 - skeletonPanel.phase) / 8)

		SkeletonBlock {
			width: parent.width * 0.45
			height: Style.fontSizeL
			visible: skeletonPanel.titleVisible
		}

		SkeletonBlock {
			width: parent.width
			height: parent.height - (skeletonPanel.titleVisible ? Style.fontSizeL + Style.marginL : 0)
			radius: Style.radiusM
			opacity: 0.5
		}
	}
}
