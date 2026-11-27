import QtQuick

Rectangle {
	id: splash

	implicitWidth: 480
	implicitHeight: 270
	color: "#2b2b2b"

	Text {
		width: splash.width
		y: 85 - height / 2
		horizontalAlignment: Text.AlignHCenter
		color: "white"
		font.pixelSize: 24
		text: Qt.application.name
	}

	Text {
		width: splash.width
		y: 175 - height / 2
		horizontalAlignment: Text.AlignHCenter
		color: "#bbbbbb"
		font.pixelSize: 13
		text: "Initializing..."
	}

	Item {
		id: spinner

		width: 24
		height: 24
		x: (splash.width - width) / 2
		y: 209 - height / 2

		RotationAnimator on rotation {
			from: 0
			to: 360
			duration: 1000
			loops: Animation.Infinite
			running: true
		}

		Repeater {
			model: 12

			Rectangle {
				width: 3
				height: 3
				radius: 1.5
				color: "white"
				opacity: (index + 1) / 12
				x: spinner.width / 2 - 1.5 + 10.5 * Math.sin(index * Math.PI / 6)
				y: spinner.height / 2 - 1.5 - 10.5 * Math.cos(index * Math.PI / 6)
			}
		}
	}
}
