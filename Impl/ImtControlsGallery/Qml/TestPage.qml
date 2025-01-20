import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtappApplicationSdl 1.0
import imtguigql 1.0
import imtauthUsersSdl 1.0

Rectangle {
	id: testPage;

	anchors.fill: parent;
	clip: true;

	UserData {
		id: userData;

		onModelChanged: {
			console.log("onModelChanged");
		}
	}

	Button {
		width: 100;
		height: 30;

		text: "Test";

		property int index: 1
		onClicked: {
			console.log("onClicked");

			index++;
			userData.m_name = "ffd" + index;
		}
	}
}
