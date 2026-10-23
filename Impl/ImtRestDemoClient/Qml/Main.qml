import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


ApplicationWindow {
	id: window

	width: 1360
	height: 900
	visible: true
	title: qsTr("imtrest demo client")
	readonly property string monoFont: Qt.platform.os === "osx" ? "Menlo" : (Qt.platform.os === "windows" ? "Consolas" : "DejaVu Sans Mono")

	header: ToolBar {
		RowLayout {
			anchors.fill: parent
			anchors.leftMargin: 10
			anchors.rightMargin: 10
			spacing: 8

			Label {
				text: qsTr("Server")
			}

			TextField {
				id: serverField

				Layout.preferredWidth: 240
				text: client.serverUrl
				selectByMouse: true

				onEditingFinished: client.serverUrl = text
			}

			Label {
				text: qsTr("Token")
			}

			TextField {
				Layout.preferredWidth: 150
				text: client.token
				selectByMouse: true

				onEditingFinished: client.token = text
			}

			CheckBox {
				text: qsTr("Capture the wire")
				checked: client.captureEnabled
				ToolTip.visible: hovered
				ToolTip.text: qsTr("The requests go through a proxy that shows the exact bytes. Without it only the Exchanges tab works.")
				onToggled: client.captureEnabled = checked
			}

			Button {
				text: qsTr("Connect")
				onClicked: {
					client.serverUrl = serverField.text
					client.testConnection()
				}
			}

			BusyIndicator {
				running: client.isBusy
				implicitHeight: 28
				implicitWidth: 28
			}

			Label {
				Layout.fillWidth: true
				text: client.statusText
				elide: Text.ElideRight
			}

			Button {
				text: qsTr("Stop all")
				enabled: client.isBusy

				onClicked: client.abortAll()
			}

			Button {
				text: qsTr("Clear logs")
				onClicked: client.clear()
			}

			ComboBox {
				id: languageBox

				Layout.preferredWidth: 120
				model: language.languages
				textRole: "name"
				currentIndex: language.languageIndex
				ToolTip.visible: hovered
				ToolTip.text: qsTr("Language of the interface")

				onActivated: function (index) {
					language.language = language.languages[index].code
				}
			}
		}
	}

	SplitView {
		anchors.fill: parent
		orientation: Qt.Vertical

		ColumnLayout {
			SplitView.preferredHeight: 380
			SplitView.minimumHeight: 200
			spacing: 0

			TabBar {
				id: tabs
				objectName: "mainTabs"

				Layout.fillWidth: true

				TabButton {
					text: qsTr("Scenarios")
				}

				TabButton {
					text: qsTr("Request")
				}

				TabButton {
					text: qsTr("Raw bytes")
				}

				TabButton {
					text: qsTr("Server info")
				}

				TabButton {
					text: qsTr("Server settings")
				}
			}

			StackLayout {
				Layout.fillWidth: true
				Layout.fillHeight: true
				currentIndex: tabs.currentIndex

				ScenariosPage {
					onLoadRequest: function (preset) {
						requestPage.load(preset)
						tabs.currentIndex = 1
					}

					onLoadRaw: function (preset) {
						rawPage.load(preset)
						tabs.currentIndex = 2
					}

					onSendRequest: function (preset) {
						requestPage.load(preset)
						preset.options.isResume === true ? requestPage.resume() : requestPage.send()
					}

					onSendRaw: function (preset) {
						rawPage.load(preset)
						rawPage.send()
					}
				}

				RequestPage {
					id: requestPage
				}

				RawPage {
					id: rawPage
				}

				ScrollView {
					TextArea {
						readOnly: true
						selectByMouse: true
						font.family: window.monoFont
						text: client.serverInfo.length > 0 ? client.serverInfo : qsTr("Press Connect: the settings of the server are shown here.")
					}
				}

				ServerSettingsPage {
				}
			}
		}

		DebugPreview {
			objectName: "debugPreview"

			SplitView.fillHeight: true
			SplitView.minimumHeight: 200
		}
	}
}
