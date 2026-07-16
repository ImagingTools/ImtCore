import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

GroupElementView {
	id: root

	property string host: ""
	property int httpPort: -1
	property int wsPort: -1
	property bool isSecure: false
	property bool readOnly: false
	property int controlWidth: Style.sizeHintM

	property alias hostInput: hostInput_
	property alias httpPortInput: httpPortInput_
	property alias wsPortInput: wsPortInput_
	property alias isSecureSwitch: switchElementView

	signal paramsChanged()

	onHostChanged: {
		hostInput_.text = host
		paramsChanged()
	}

	onHttpPortChanged: {
		httpPortInput_.value = httpPort
		paramsChanged()
	}

	onWsPortChanged: {
		wsPortInput_.value = wsPort
		paramsChanged()
	}

	onIsSecureChanged: {
		switchElementView.checked = isSecure
		paramsChanged()
	}

	TextInputElementView {
		id: hostInput_
		controlWidth: root.controlWidth
		name: qsTr("Server Host")
		description: qsTr("Host name or IP address used by clients to reach the service")
		readOnly: root.readOnly
		onEditingFinished: {
			root.host = text
		}
	}

	IntegerInputElementView {
		id: httpPortInput_
		controlWidth: root.controlWidth
		name: qsTr("Http Port")
		description: qsTr("Port used for HTTP service requests")
		readOnly: root.readOnly
		onEditingFinished: {
			root.httpPort = value
		}
	}

	IntegerInputElementView {
		id: wsPortInput_
		controlWidth: root.controlWidth
		name: qsTr("Web Socket Port")
		description: qsTr("Port used for WebSocket connections and subscriptions")
		readOnly: root.readOnly
		onEditingFinished: {
			root.wsPort = value
		}
	}
	
	SwitchElementView {
		id: switchElementView
		controlWidth: root.controlWidth
		name: qsTr("Secure Connection")
		description: qsTr("Uses encrypted HTTPS and secure WebSocket connections")
		readOnly: root.readOnly
		onCheckedChanged: {
			root.isSecure = checked
		}
	}
}

