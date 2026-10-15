import QtQuick 2.12
import Acf 1.0

// Placeholder shape for content that is still loading.
Rectangle {
	id: skeletonBlock

	// A block on its own pulses by itself; inside SkeletonFormView the whole form pulses instead.
	property bool animated: false
	property int phase: 0

	radius: Style.radiusS
	color: Style.neutralSubtleColor
	opacity: skeletonBlock.animated ? 0.45 + 0.55 * (skeletonBlock.phase < 8 ? skeletonBlock.phase / 8 : (16 - skeletonBlock.phase) / 8) : 1

	Timer {
		interval: 80
		repeat: true
		running: skeletonBlock.animated && skeletonBlock.visible

		onTriggered: {
			skeletonBlock.phase = (skeletonBlock.phase + 1) % 16
		}
	}
}
