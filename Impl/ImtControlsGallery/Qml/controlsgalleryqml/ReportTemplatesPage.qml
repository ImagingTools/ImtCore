import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0
import EjEditor 1.0 as Ed

Rectangle {
	id: root

	anchors.fill: parent
	clip: true
	color: Style.baseColor

	// Hosts the EjQml editor demo (3rdParty/EjQml/qml/EjEditor/DemoPage.qml).
	Ed.DemoPage {
		anchors.fill: parent
	}
}
