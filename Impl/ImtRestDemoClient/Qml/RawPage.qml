import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


// The bytes that are written to the socket as they are: the requests that a client library does not make (in parts, with a pause,
// wrong, repeated headers, a request that is not finished). The answer is shown in the debug preview.
Item {
	id: root

	readonly property string monoFont: Qt.platform.os === "osx" ? "Menlo" : (Qt.platform.os === "windows" ? "Consolas" : "DejaVu Sans Mono")

	function load(preset) {
		var options = preset.options
		textArea.text = options.text || ""
		autoCrlfCheck.checked = options.autoCrlf !== false
		partSizeField.text = options.partSize !== undefined ? String(options.partSize) : "0"
		delayField.text = options.delayMs !== undefined ? String(options.delayMs) : "0"
		idleField.text = options.idleTimeoutMs !== undefined ? String(options.idleTimeoutMs) : "2000"
		noteLabel.text = preset.description || ""
	}

	function getOptions() {
		return {
			"text": textArea.text,
			"autoCrlf": autoCrlfCheck.checked,
			"partSize": Number(partSizeField.text),
			"delayMs": Number(delayField.text),
			"idleTimeoutMs": Number(idleField.text)
		}
	}

	function send() {
		client.sendRaw(getOptions())
	}

	RowLayout {
		anchors.fill: parent
		anchors.margins: 10
		spacing: 14

		ColumnLayout {
			Layout.fillWidth: true
			Layout.fillHeight: true
			Layout.preferredWidth: 2

			Label {
				text: qsTr("Bytes of the request. %1 and %2 are replaced by the bytes.").arg("\\r \\n \\t \\\\").arg("\\xNN")
			}

			ScrollView {
				Layout.fillWidth: true
				Layout.fillHeight: true

				TextArea {
					id: textArea

					font.family: root.monoFont
					text: "GET /echo HTTP/1.1\nHost: localhost\nConnection: close\n\n"
					selectByMouse: true
					background: Rectangle {
						border.color: "#b0b0b0"
					}
				}
			}
		}

		ColumnLayout {
			Layout.fillWidth: true
			Layout.fillHeight: true
			Layout.preferredWidth: 1

			CheckBox {
				id: autoCrlfCheck

				text: qsTr("A line break is CR LF")
				checked: true
			}

			GridLayout {
				columns: 2

				Label {
					text: qsTr("Part size, bytes")
				}

				TextField {
					id: partSizeField

					Layout.preferredWidth: 90
					text: "0"
					selectByMouse: true
					ToolTip.visible: hovered
					ToolTip.text: qsTr("0 - all bytes at once")
				}

				Label {
					text: qsTr("Pause between the parts, ms")
				}

				TextField {
					id: delayField

					Layout.preferredWidth: 90
					text: "0"
					selectByMouse: true
				}

				Label {
					text: qsTr("Close after the silence, ms")
				}

				TextField {
					id: idleField

					Layout.preferredWidth: 90
					text: "2000"
					selectByMouse: true
					ToolTip.visible: hovered
					ToolTip.text: qsTr("The connection is closed when everything is sent and the server is silent so long")
				}
			}

			Label {
				id: noteLabel

				Layout.fillWidth: true
				wrapMode: Text.WordWrap
				color: "#555555"
			}

			Button {
				text: qsTr("Send")
				highlighted: true

				onClicked: root.send()
			}

			Item {
				Layout.fillHeight: true
			}
		}
	}
}
