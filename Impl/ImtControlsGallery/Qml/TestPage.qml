import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
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

	BaseModel {
		id: baseModel;
	}

	UserData {
		id: userData;

		onModelChanged: {
			console.log("onModelChanged");
		}
	}

	SystemInfo {
		id: systemInfo;
	}

	Row {
		height: 30;

		Button {
			width: 100;
			height: 30;

			text: "owner";

			property int index: 1
			onClicked: {
				console.log("onClicked");

				index++;

				userData.m_systemInfos.addElement(systemInfo);
			}
		}

		Button {
			width: 100;
			height: 30;

			text: "changeList";

			property int index: 1
			onClicked: {
				console.log("onClicked");

				index++;

				userData.beginChanges();
				userData.m_roles = "rewrwer;werwerwe;rwer";
				userData.m_name = "Test";
				userData.m_username = "Test";
				userData.endChanges();
			}
		}
	}

	Rectangle{
		id: rec

		x: 400;
		y: 400;

		width: 100;
		height: 100;

		color: "red";

		MovingItem{
			containerItem: rec;
			movingItem: rec;

			hoverEnabled: true;
			cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor
		}
	}


}
