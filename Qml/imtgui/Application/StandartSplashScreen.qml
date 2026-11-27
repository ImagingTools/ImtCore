import QtQuick 2.12

Item {
	id: splashScreen

	implicitWidth: 640
	implicitHeight: 360

	property url backgroundSource: "qrc:/qml/imtgui/Resources/Images/StandartSplashScreen.png"
	property string productName: ""
	property string applicationName: ""
	property string applicationType: ""
	property string mainVersion: ""
	property var versionDescriptions: []
	property var versionTexts: []
	property string copyright: ""
	property string progressText: ""

	Rectangle {
		anchors.fill: parent
		color: "#0a1430"
	}

	Image {
		id: backgroundImage

		anchors.fill: parent
		source: splashScreen.backgroundSource
		fillMode: Image.Stretch
		smooth: true
		mipmap: true
	}

	Column {
		id: titleColumn

		x: 20
		y: 14
		spacing: 4

		Text {
			id: productNameText

			visible: splashScreen.productName !== ""
			color: "white"
			font.pixelSize: 32
			text: splashScreen.productName
		}

		Text {
			id: applicationNameText

			visible: splashScreen.applicationName !== ""
			color: "white"
			font.family: "Arial"
			font.pixelSize: 21
			text: splashScreen.applicationName
		}

		Text {
			id: applicationTypeText

			visible: splashScreen.applicationType !== ""
			color: "white"
			font.family: "Arial"
			font.pixelSize: 16
			text: splashScreen.applicationType
		}
	}

	Column {
		id: infoColumn

		x: 20
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 42
		spacing: 6

		Row {
			id: versionsRow

			spacing: 14

			Column {
				id: versionDescriptionColumn

				spacing: 2

				Text {
					id: mainVersionTitleText

					visible: splashScreen.mainVersion !== ""
					color: "white"
					font.pixelSize: 13
					font.bold: true
					text: qsTr("Version")
				}

				Repeater {
					id: versionDescriptionRepeater

					model: splashScreen.versionDescriptions

					Text {
						color: "white"
						font.pixelSize: 13
						font.bold: true
						text: modelData
					}
				}
			}

			Column {
				id: versionValueColumn

				spacing: 2

				Text {
					id: mainVersionText

					visible: splashScreen.mainVersion !== ""
					color: "white"
					font.pixelSize: 13
					font.bold: true
					text: splashScreen.mainVersion
				}

				Repeater {
					id: versionValueRepeater

					model: splashScreen.versionTexts

					Text {
						color: "white"
						font.pixelSize: 13
						font.bold: true
						text: modelData
					}
				}
			}
		}

		Text {
			id: copyrightText

			visible: splashScreen.copyright !== ""
			color: "#ffff80"
			font.pixelSize: 13
			text: splashScreen.copyright
		}
	}

	Text {
		id: progressTextItem

		x: 20
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 16
		width: splashScreen.width - 40
		elide: Text.ElideRight
		color: "white"
		font.pixelSize: 13
		text: splashScreen.progressText
	}
}
