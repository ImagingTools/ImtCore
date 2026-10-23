import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


// The settings of the running server: the same that the server gets from the command line. The server is asked through /admin/settings
// (the token is needed), after "Apply" it restarts with the new settings and the client follows it: a new port and a new token are taken
// by the client at once. If the server cannot start with the new settings (the port is busy) it goes back to the previous ones.
Item {
	id: root

	readonly property string monoFont: Qt.platform.os === "osx" ? "Menlo" : (Qt.platform.os === "windows" ? "Consolas" : "DejaVu Sans Mono")

	readonly property var settings: client.serverSettings
	readonly property bool isLoaded: settings.port !== undefined

	// The presets: the sets of the test certificates that the server has (it is started with --tls-samples)
	readonly property var sampleModel: isLoaded && settings.tlsSamples ? settings.tlsSamples : []

	function splitFiles(text) {
		return text.split(";").map(function (item) { return item.trim() }).filter(function (item) { return item.length > 0 })
	}

	// What the client trusts besides the paths of the field: the certificates of the presets are kept as the text
	function trustedTexts() {
		return client.trustedCertificates.filter(function (item) { return item.indexOf("-----BEGIN") === 0 })
	}

	// One click: the server uses the preset, the client trusts its certificates
	function applyPreset() {
		if (sampleBox.currentIndex < 0 || sampleBox.currentIndex >= sampleModel.length) {
			return
		}

		var sample = sampleModel[sampleBox.currentIndex]
		client.applyTlsPreset(sample.id, sample.trustedPem)
	}

	// The files of this computer go to the server, the client trusts the files of the field (or the certificate itself, if the field is empty)
	function sendFiles() {
		var trusted = splitFiles(trustField.text)
		if (trusted.length === 0) {
			trusted = [localCertField.text.trim()]
		}

		client.applyTlsFiles(localCertField.text.trim(), splitFiles(localChainField.text), localKeyField.text.trim(), trusted)
	}

	// The values that are in the fields, as the server wants them
	function currentValues() {
		return {
			"port": portBox.value,
			"token": tokenField.text,
			"threadsLimit": threadsBox.value,
			"requestInactivityTimeoutMs": inactivityBox.value,
			"keepAliveTimeoutMs": keepAliveBox.value,
			"requestProcessingTimeoutMs": processingBox.value,
			"writeHighWatermark": watermarkBox.value,
			"maxBodySize": maxBodyBox.value,
			"isLogEnabled": logBox.checked,
			"tlsEnabled": tlsBox.checked,
			"tlsCertificateFile": certField.text,
			"tlsChainFiles": splitFiles(chainField.text),
			"tlsKeyFile": keyField.text
		}
	}

	// The fields get the values that the server has
	function fill() {
		// The handler can run before the property settings gets the new value
		var values = client.serverSettings
		if (values.port === undefined) {
			return
		}

		portBox.value = values.port
		tokenField.text = values.token
		threadsBox.value = values.threadsLimit
		inactivityBox.value = values.requestInactivityTimeoutMs
		keepAliveBox.value = values.keepAliveTimeoutMs
		processingBox.value = values.requestProcessingTimeoutMs
		watermarkBox.value = values.writeHighWatermark
		maxBodyBox.value = values.maxBodySize
		logBox.checked = values.isLogEnabled
		tlsBox.checked = values.tlsEnabled
		certField.text = values.tlsCertificateFile
		chainField.text = values.tlsChainFiles.join(";")
		keyField.text = values.tlsKeyFile
		fillTrust()
	}

	function fillTrust() {
		trustField.text = client.trustedCertificates.filter(function (item) { return item.indexOf("-----BEGIN") !== 0 }).join(";")
	}

	// Only the settings that are changed in the fields are sent
	function changes() {
		var values = currentValues()
		var result = {}

		for (var name in values) {
			if (JSON.stringify(values[name]) !== JSON.stringify(settings[name])) {
				result[name] = values[name]
			}
		}

		return result
	}

	function apply() {
		var changed = changes()
		var isTlsChanged = false

		for (var name in changed) {
			if (name.indexOf("tls") === 0) {
				isTlsChanged = true
			}
		}

		// The client trusts only the certificates of the list. When the TLS of the server is changed, the client trusts the old and the new
		// ones while the server restarts, and only the new ones after that
		if (isTlsChanged) {
			client.applyTlsSettings(changed, splitFiles(trustField.text))
		} else {
			client.trustedCertificates = trustedTexts().concat(splitFiles(trustField.text))
			client.applyServerSettings(changed)
		}
	}

	Connections {
		target: client

		function onServerSettingsChanged() {
			root.fill()
		}

		function onTrustedCertificatesChanged() {
			root.fillTrust()
		}
	}

	onVisibleChanged: {
		if (visible && !isLoaded) {
			client.loadServerSettings()
		}
	}

	ScrollView {
		anchors.fill: parent
		contentWidth: availableWidth

		ColumnLayout {
			width: parent.width
			spacing: 6

			RowLayout {
				Layout.leftMargin: 10
				Layout.rightMargin: 10
				Layout.topMargin: 6
				spacing: 8

				Button {
					text: qsTr("Read from the server")

					onClicked: client.loadServerSettings()
				}

				Button {
					text: qsTr("Apply")
					highlighted: true
					enabled: root.isLoaded

					onClicked: root.apply()
				}

				Button {
					text: qsTr("Cancel changes")
					enabled: root.isLoaded

					onClicked: root.fill()
				}

				Label {
					Layout.fillWidth: true
					text: client.settingsStatus
					elide: Text.ElideRight
					font.bold: true
				}
			}

			Label {
				Layout.leftMargin: 10
				Layout.rightMargin: 10
				Layout.fillWidth: true
				wrapMode: Text.WordWrap
				color: "#555555"
				text: qsTr("The settings are the options of the command line of the server. After Apply the server restarts (the open connections are closed), a new port and a new token are taken by this client at once. If the server cannot start with the new settings, it goes back to the previous ones and shows the reason.")
			}

			GridLayout {
				Layout.leftMargin: 10
				Layout.rightMargin: 10
				Layout.fillWidth: true
				columns: 3
				columnSpacing: 12
				rowSpacing: 4
				enabled: root.isLoaded

				Label {
					text: qsTr("Port")
				}

				SpinBox {
					id: portBox

					from: 1
					to: 65535
					editable: true
					textFromValue: function (value) { return value.toString() }
				}

				Label {
					text: "--port"
					color: "#777777"
				}

				Label {
					text: qsTr("Token")
				}

				TextField {
					id: tokenField

					Layout.preferredWidth: 220
					selectByMouse: true
				}

				Label {
					text: "--token"
					color: "#777777"
				}

				Label {
					text: qsTr("Threads of the handlers")
				}

				SpinBox {
					id: threadsBox

					from: 1
					to: 256
					editable: true
				}

				Label {
					text: "--threads"
					color: "#777777"
				}

				Label {
					text: qsTr("Inactivity timeout, ms")
				}

				SpinBox {
					id: inactivityBox

					from: 0
					to: 3600000
					stepSize: 1000
					editable: true
					textFromValue: function (value) { return value.toString() }
				}

				Label {
					text: qsTr("--inactivity-timeout (0 - no limit)")
					color: "#777777"
				}

				Label {
					text: qsTr("Keep-alive timeout, ms")
				}

				SpinBox {
					id: keepAliveBox

					from: 0
					to: 3600000
					stepSize: 1000
					editable: true
					textFromValue: function (value) { return value.toString() }
				}

				Label {
					text: qsTr("--keep-alive-timeout (0 - no limit)")
					color: "#777777"
				}

				Label {
					text: qsTr("Processing timeout, ms")
				}

				SpinBox {
					id: processingBox

					from: 0
					to: 3600000
					stepSize: 1000
					editable: true
					textFromValue: function (value) { return value.toString() }
				}

				Label {
					text: qsTr("--processing-timeout (0 - no limit)")
					color: "#777777"
				}

				Label {
					text: qsTr("Write high watermark, bytes")
				}

				SpinBox {
					id: watermarkBox

					from: 1024
					to: 268435456
					stepSize: 65536
					editable: true
					textFromValue: function (value) { return value.toString() }
				}

				Label {
					text: qsTr("the size of the queue of the response of a connection")
					color: "#777777"
				}

				Label {
					text: qsTr("Limit of the body in memory, bytes")
				}

				SpinBox {
					id: maxBodyBox

					from: 0
					to: 2147483647
					stepSize: 1024
					editable: true
					textFromValue: function (value) { return value.toString() }
				}

				Label {
					text: qsTr("--max-body-size (0 - no limit)")
					color: "#777777"
				}

				Label {
					text: qsTr("Log of the server")
				}

				CheckBox {
					id: logBox

					text: qsTr("Write the messages of the components to the output of the server")
				}

				Label {
					text: "--log"
					color: "#777777"
				}

				Label {
					text: qsTr("TLS")
				}

				CheckBox {
					id: tlsBox

					text: qsTr("Encrypt the connections (https)")
				}

				Label {
					text: "--tls"
					color: "#777777"
				}

				Label {
					text: qsTr("Preset of the server")
				}

				RowLayout {
					spacing: 8

					ComboBox {
						id: sampleBox

						Layout.preferredWidth: 420
						model: root.sampleModel
						textRole: "title"
						enabled: root.sampleModel.length > 0
						ToolTip.visible: hovered && currentIndex >= 0 && currentIndex < root.sampleModel.length
						ToolTip.text: currentIndex >= 0 && currentIndex < root.sampleModel.length ? root.sampleModel[currentIndex].description : ""
					}

					Button {
						text: qsTr("Apply preset")
						enabled: root.sampleModel.length > 0

						onClicked: root.applyPreset()
					}
				}

				Label {
					text: qsTr("one click: the server uses the set, this client trusts it")
					color: "#777777"
				}

				Label {
					text: qsTr("Certificate file on the server")
				}

				TextField {
					id: certField

					Layout.fillWidth: true
					selectByMouse: true
					font.family: root.monoFont
				}

				Label {
					text: qsTr("--tls-cert (can have the whole chain)")
					color: "#777777"
				}

				Label {
					text: qsTr("Intermediate certificates on the server")
				}

				TextField {
					id: chainField

					Layout.fillWidth: true
					selectByMouse: true
					font.family: root.monoFont
					placeholderText: qsTr("files separated by ;")
				}

				Label {
					text: "--tls-chain"
					color: "#777777"
				}

				Label {
					text: qsTr("Private key file on the server")
				}

				TextField {
					id: keyField

					Layout.fillWidth: true
					selectByMouse: true
					font.family: root.monoFont
				}

				Label {
					text: qsTr("--tls-key (the files are applied by the button Apply)")
					color: "#777777"
				}

				Label {
					text: qsTr("Certificate of this computer")
				}

				TextField {
					id: localCertField

					Layout.fillWidth: true
					selectByMouse: true
					font.family: root.monoFont
					placeholderText: qsTr("PEM file, for example a real certificate")
				}

				Label {
					text: qsTr("the content of the files goes to the server")
					color: "#777777"
				}

				Label {
					text: qsTr("Intermediate certificates of this computer")
				}

				TextField {
					id: localChainField

					Layout.fillWidth: true
					selectByMouse: true
					font.family: root.monoFont
					placeholderText: qsTr("files separated by ;")
				}

				Label {
					text: ""
				}

				Label {
					text: qsTr("Key of this computer")
				}

				RowLayout {
					spacing: 8

					TextField {
						id: localKeyField

						Layout.fillWidth: true
						selectByMouse: true
						font.family: root.monoFont
						placeholderText: qsTr("PEM file without a password")
					}

					Button {
						text: qsTr("Send to the server")
						enabled: localCertField.text.length > 0 && localKeyField.text.length > 0

						onClicked: root.sendFiles()
					}
				}

				Label {
					text: qsTr("the key is kept by the server in its own directory")
					color: "#777777"
				}

				Label {
					text: qsTr("This client trusts")
				}

				TextField {
					id: trustField

					Layout.fillWidth: true
					selectByMouse: true
					font.family: root.monoFont
					placeholderText: qsTr("files separated by ; (when sending: empty means the certificate file itself)")
				}

				Label {
					text: qsTr("only these certificates; the system is not used")
					color: "#777777"
				}

				Label {
					text: qsTr("Directory of the files")
				}

				Label {
					Layout.columnSpan: 2
					Layout.fillWidth: true
					text: root.isLoaded ? root.settings.filesDirectory : ""
					font.family: root.monoFont
					elide: Text.ElideMiddle
				}

				Label {
					text: qsTr("Restarts")
				}

				Label {
					Layout.columnSpan: 2
					text: root.isLoaded ? qsTr("%1 (the last error: %2)").arg(root.settings.restartCount).arg(root.settings.lastError.length > 0 ? root.settings.lastError : qsTr("none")) : ""
				}
			}
		}
	}
}
