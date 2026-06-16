import QtQuick 2.15
import QtQuick.Window 2.15

Window {
	id: root
	width: 920
	height: 560
	minimumWidth: 700
	minimumHeight: 420
	visible: true
	title: "HostTest - Information"
	color: "#1e1e1e"

	Rectangle {
		anchors.fill: parent
		color: "#1e1e1e"

		Rectangle {
			anchors.centerIn: parent
			width: Math.min(parent.width - 48, 860)
			height: Math.min(parent.height - 48, 500)
			radius: 10
			color: "#252526"
			border.color: "#3c3c3c"
			border.width: 1

			Column {
				anchors.fill: parent
				anchors.margins: 24
				spacing: 14

				Text {
					text: "HostTest"
					color: "#ffffff"
					font.pixelSize: 30
					font.bold: true
				}

				Text {
					width: parent.width
					wrapMode: Text.WordWrap
					color: "#d4d4d4"
					font.pixelSize: 16
					text: "This window is shown because HostTest was started without a QML file argument."
				}

				Text {
					width: parent.width
					wrapMode: Text.WordWrap
					color: "#d4d4d4"
					font.pixelSize: 16
					text: "Why it does not close automatically: in this mode HostTest displays information and keeps the Qt event loop running until you close the window manually."
				}

				Text {
					width: parent.width
					wrapMode: Text.WordWrap
					color: "#d4d4d4"
					font.pixelSize: 16
					text: "What this application is for: HostTest is a desktop QML host for running arbitrary QML files (including tests) with ImtCore initialisation and registered QML/C++ types."
				}

				Rectangle {
					width: parent.width
					height: 1
					color: "#3c3c3c"
				}

				Text {
					width: parent.width
					wrapMode: Text.WordWrap
					color: "#9cdcfe"
					font.pixelSize: 15
					text: "CLI examples:\n  HostTest.exe --qml-file <path-to-qml>\n  HostTest.exe <path-to-qml>\n  HostTest.exe -I <import-path> <path-to-qml>"
				}
			}
		}
	}
}
