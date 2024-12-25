import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtappApplicationSdl 1.0
import imtguigql 1.0

Rectangle {
	id: testPage;

	anchors.fill: parent;
	clip: true;

	Component.onCompleted: {
		rectComp.createObject(testPage)
	}

	function onCompleted(){
		console.log("Function onCompleted");
	}

	Connections {
		target: rectComp;

		function onCompleted(){
			console.log("Connections onCompleted");
		}

		function onTest(){
			console.log("Connections onTest");
		}
	}

	Component {
		id: rectComp;

		Rectangle {
			width: 100;
			height: 100;
			color: "red";

			signal test();

			Component.onCompleted: {
				test()
				console.log("Rectangle onCompleted");
			}
		}
	}
}
