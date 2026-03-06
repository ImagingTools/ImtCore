import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0

Rectangle {
	id: testPage;

	anchors.fill: parent;
	clip: true;
	color: Style.baseColor

	Graph2d {
		id: graph2d
		anchors.fill: parent;
		hasData: true
		gridStepMajorX: 1
		gridStepMajorY: 200
		alwaysShowOrigin: true
		xScale: 2
		hasMinorGrid: false 
		hasTooltip: false
		fitToWidth: true
		legendFontSize: Style.fontSizeS
		labelFontSize: Style.fontSizeS
	}

	Button {
		text: "Apply Data"
		onClicked: {
			graph2d.legendX = "Test"
			graph2d.legendY = "Test"
			graph2d.labelXValues = ["2025"]
			graph2d.linePoints = [Qt.point(0, 0)]
			graph2d.requestPaint()
		}
	}
}
