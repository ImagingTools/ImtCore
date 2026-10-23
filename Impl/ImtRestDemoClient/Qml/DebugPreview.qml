import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


// Debug preview: what the application has sent to the server and what has come back.
// - Exchanges: the requests and the responses as the application sees them (the method, the headers, the beginning of the bodies,
//   the time, the check of the body);
// - Wire: the exact bytes of the connections, taken by the proxy: everything that the client library has added (the headers,
//   the framing of the chunks, Expect: 100-continue, the interim responses) and the events of the connections.
Item {
	id: root

	property alias currentTab: tabs.currentIndex

	readonly property string monoFont: Qt.platform.os === "osx" ? "Menlo" : (Qt.platform.os === "windows" ? "Consolas" : "DejaVu Sans Mono")

	TextEdit {
		id: clipboardHelper

		visible: false
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		TabBar {
			id: tabs

			Layout.fillWidth: true

			TabButton {
				text: qsTr("Exchanges (%1)").arg(client.exchanges.count)
			}

			TabButton {
				text: qsTr("Wire (%1)").arg(client.wire.count)
			}
		}

		StackLayout {
			Layout.fillWidth: true
			Layout.fillHeight: true
			currentIndex: tabs.currentIndex

			// Exchanges
			SplitView {
				id: exchangesView

				property int selectedRow: -1

				function refresh() {
					var row = exchangesView.selectedRow
					summaryText.text = row >= 0 ? client.exchanges.summaryText(row) : ""
					requestText.text = row >= 0 ? client.exchanges.requestText(row) : ""
					responseText.text = row >= 0 ? client.exchanges.responseText(row) : ""
				}

				orientation: Qt.Horizontal

				Connections {
					target: client.exchanges

					function onCountChanged() {
						// The last exchange is shown, if the user has not chosen another one
						if (followCheck.checked || client.exchanges.count === 0) {
							exchangesView.selectedRow = client.exchanges.count - 1
							exchangeList.positionViewAtEnd()
						}

						exchangesView.refresh()
					}

					function onDataChanged() {
						exchangesView.refresh()
					}

					function onModelReset() {
						exchangesView.selectedRow = -1
						exchangesView.refresh()
					}
				}

				ColumnLayout {
					SplitView.preferredWidth: 430
					SplitView.minimumWidth: 250

					CheckBox {
						id: followCheck

						text: qsTr("Show the last exchange")
						checked: true
					}

					ListView {
						id: exchangeList

						Layout.fillWidth: true
						Layout.fillHeight: true
						clip: true
						model: client.exchanges
						currentIndex: exchangesView.selectedRow

						ScrollBar.vertical: ScrollBar {
						}

						delegate: ItemDelegate {
							id: exchangeDelegate

							required property int index
							required property int exchangeId
							required property string method
							required property string url
							required property int statusCode
							required property bool isFinished
							required property bool hasError
							required property real durationMs
							required property string speedText

							width: ListView.view.width
							highlighted: ListView.isCurrentItem

							contentItem: RowLayout {
								spacing: 8

								Label {
									text: "#" + exchangeDelegate.exchangeId
									color: "#777777"
								}

								Label {
									text: exchangeDelegate.method
									font.bold: true
									Layout.preferredWidth: 56
								}

								Label {
									Layout.fillWidth: true
									text: exchangeDelegate.url.replace(/^https?:\/\/[^\/]+/, "")
									elide: Text.ElideMiddle
								}

								Label {
									text: !exchangeDelegate.isFinished ? "..." : (exchangeDelegate.statusCode > 0 ? exchangeDelegate.statusCode : qsTr("no answer"))
									font.bold: true
									color: !exchangeDelegate.isFinished ? "#777777" : (exchangeDelegate.hasError ? "#c0392b" : "#1e8449")
								}

								Label {
									text: exchangeDelegate.speedText
									color: exchangeDelegate.isFinished ? "#777777" : "#1f5a9e"
									font.bold: !exchangeDelegate.isFinished
								}

								Label {
									text: exchangeDelegate.isFinished ? exchangeDelegate.durationMs + " ms" : ""
									color: "#777777"
								}
							}

							onClicked: {
								followCheck.checked = false
								exchangesView.selectedRow = exchangeDelegate.index
								exchangesView.refresh()
							}
						}
					}
				}

				ScrollView {
					id: detailsScroll

					SplitView.fillWidth: true
					contentWidth: availableWidth

					ColumnLayout {
						width: detailsScroll.availableWidth
						spacing: 4

						Label {
							text: qsTr("Summary")
							font.bold: true
						}

						TextArea {
							id: summaryText

							Layout.fillWidth: true
							readOnly: true
							selectByMouse: true
							wrapMode: Text.WrapAnywhere
							font.family: root.monoFont
							background: Rectangle {
								color: "#f5f5f5"
							}
						}

						Label {
							text: qsTr("Request, as the application asked (the exact bytes are in the Wire tab)")
							font.bold: true
						}

						TextArea {
							id: requestText

							Layout.fillWidth: true
							readOnly: true
							selectByMouse: true
							wrapMode: Text.WrapAnywhere
							font.family: root.monoFont
							background: Rectangle {
								color: "#eef4fb"
							}
						}

						Label {
							text: qsTr("Response")
							font.bold: true
						}

						TextArea {
							id: responseText

							Layout.fillWidth: true
							readOnly: true
							selectByMouse: true
							wrapMode: Text.WrapAnywhere
							font.family: root.monoFont
							background: Rectangle {
								color: "#eef8f0"
							}
						}
					}
				}
			}

			// Wire
			ColumnLayout {
				RowLayout {
					Layout.fillWidth: true

					CheckBox {
						id: wireFollowCheck

						text: qsTr("Follow")
						checked: true
					}

					Button {
						text: qsTr("Copy all")
						onClicked: {
							clipboardHelper.text = client.wire.getAllText()
							clipboardHelper.selectAll()
							clipboardHelper.copy()
						}
					}

					Label {
						Layout.fillWidth: true
						text: client.captureEnabled
							? qsTr("The bytes that were exchanged with the server: blue is sent, green is received. %1 is a line break of the protocol.").arg("\\r\\n")
							: qsTr("The capture of the wire is off (switch it on in the toolbar).")
						color: "#555555"
						elide: Text.ElideRight
					}
				}

				ListView {
					id: wireList

					Layout.fillWidth: true
					Layout.fillHeight: true
					clip: true
					model: client.wire
					spacing: 4

					ScrollBar.vertical: ScrollBar {
					}

					onCountChanged: {
						if (wireFollowCheck.checked) {
							wireList.positionViewAtEnd()
						}
					}

					delegate: Rectangle {
						id: wireDelegate

						required property int connectionId
						required property int kind
						required property string time
						required property real size
						required property string text
						required property bool isTruncated

						width: ListView.view.width
						height: wireColumn.implicitHeight + 8
						color: kind === 0 ? "#e6f0fb" : (kind === 1 ? "#e6f6ea" : "#f0f0f0")
						border.color: kind === 0 ? "#9bbbe0" : (kind === 1 ? "#9fd2ac" : "#cccccc")

						Column {
							id: wireColumn

							x: 8
							y: 4
							width: parent.width - 16

							Label {
								width: parent.width
								font.bold: true
								color: wireDelegate.kind === 0 ? "#1f5a9e" : (wireDelegate.kind === 1 ? "#1e7a3a" : "#666666")
								text: wireDelegate.kind === 2
									? wireDelegate.time + "   " + wireDelegate.text
									: wireDelegate.time + "   #" + wireDelegate.connectionId + (wireDelegate.kind === 0 ? "   client -> server   " : "   server -> client   ") + wireDelegate.size + " bytes"
							}

							Label {
								width: parent.width
								visible: wireDelegate.kind !== 2
								text: wireDelegate.text
								wrapMode: Text.WrapAnywhere
								font.family: root.monoFont
								font.pixelSize: 12
							}
						}
					}
				}
			}
		}
	}
}
