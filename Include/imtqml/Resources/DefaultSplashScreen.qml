import QtQuick
import Acf 1.0

Rectangle {
	id: splash

	implicitWidth: Math.max(480, titleText.implicitWidth + 48)
	implicitHeight: 270
	color: Style.backgroundColor

	Text {
		id: titleText

		width: splash.width
		y: 85 - height / 2
		horizontalAlignment: Text.AlignHCenter
		color: Style.textColor
		font.family: Style.fontFamily
		font.pixelSize: Style.fontSizeXXL
		text: Qt.application.name
	}

	Text {
		width: splash.width
		y: 175 - height / 2
		horizontalAlignment: Text.AlignHCenter
		color: Style.inactiveTextColor
		font.family: Style.fontFamily
		font.pixelSize: Style.fontSizeM
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
				color: Style.textColor
				opacity: (index + 1) / 12
				x: spinner.width / 2 - 1.5 + 10.5 * Math.sin(index * Math.PI / 6)
				y: spinner.height / 2 - 1.5 - 10.5 * Math.cos(index * Math.PI / 6)
			}
		}
	}
}
