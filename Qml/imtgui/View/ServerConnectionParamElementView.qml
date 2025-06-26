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
		name: qsTr("Server Host")
		readOnly: root.readOnly
		onEditingFinished: {
			root.host = text
		}
	}

	IntegerInputElementView {
		id: httpPortInput_
		name: qsTr("Http Port")
		readOnly: root.readOnly
		onEditingFinished: {
			root.httpPort = value
		}
	}

	IntegerInputElementView {
		id: wsPortInput_
		name: qsTr("Web Socket Port")
		readOnly: root.readOnly
		onEditingFinished: {
			root.wsPort = value
		}
	}
	
	SwitchElementView {
		id: switchElementView
		name: qsTr("Secure Connection")
		readOnly: root.readOnly
		onCheckedChanged: {
			root.isSecure = checked
		}
	}
}

