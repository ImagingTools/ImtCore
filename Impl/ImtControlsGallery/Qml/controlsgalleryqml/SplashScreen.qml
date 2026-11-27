import QtQuick
import QtQuick.Window

Item {
	id: splash

	implicitWidth: 480
	implicitHeight: 270

	property real phase: 0

	NumberAnimation on phase {
		from: 0
		to: 1
		duration: 1400
		loops: Animation.Infinite
		running: true
	}

	Rectangle {
		anchors.fill: parent
		radius: 10

		gradient: Gradient {
			orientation: Gradient.Horizontal
			GradientStop { position: 0.0; color: "#0f2027" }
			GradientStop { position: 0.5; color: "#2c5364" }
			GradientStop { position: 1.0; color: "#5b2a86" }
		}

		Rectangle {
			width: parent.width
			height: 4
			anchors.top: parent.top
			gradient: Gradient {
				orientation: Gradient.Horizontal
				GradientStop { position: 0.0; color: "#00c6ff" }
				GradientStop { position: 0.5; color: "#7f5af0" }
				GradientStop { position: 1.0; color: "#ff4ecd" }
			}
		}

		Text {
			width: parent.width
			y: 85 - height / 2
			horizontalAlignment: Text.AlignHCenter
			color: "white"
			font.pixelSize: 28
			font.bold: true
			text: Qt.application.name
		}

		Text {
			width: parent.width
			y: 125 - height / 2
			horizontalAlignment: Text.AlignHCenter
			color: "#9fd8ff"
			font.pixelSize: 13
			text: "ImtCore QML controls gallery"
		}

		Text {
			width: parent.width
			y: 175 - height / 2
			horizontalAlignment: Text.AlignHCenter
			color: "#d0d0e0"
			font.pixelSize: 13
			text: "Initializing..."
		}

		Row {
			id: dotsRow
			spacing: 10
			x: (parent.width - width) / 2
			y: 209 - height / 2

			Repeater {
				model: 5

				Rectangle {
					property real wave: Math.max(0, Math.cos((splash.phase - index / 5) * 2 * Math.PI))

					width: 10
					height: 10
					radius: 5
					anchors.verticalCenter: parent.verticalCenter
					scale: 0.5 + 0.7 * wave
					opacity: 0.35 + 0.65 * wave
					color: Qt.hsla(0.52 + index * 0.1, 0.9, 0.65, 1)
				}
			}
		}
	}
}
