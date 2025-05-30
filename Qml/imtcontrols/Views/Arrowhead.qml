import QtQuick 2.0
import Acf 1.0


Canvas {
	id: canvas;

	width: Style.marginXL;
	height: Style.marginXL;

	antialiasing: true;

	property int gap: 0
	property string color: Style.baseColor;
	property bool isConcave: false;

	Component.onCompleted: {
		requestPaint();
	}
	onWidthChanged: {
		requestPaint();
	}
	onHeightChanged: {
		requestPaint();
	}

	onPaint: {
		var ctx = canvas.getContext('2d');
		ctx.reset();
		ctx.clearRect(0, 0, canvas.width, canvas.height);

		ctx.lineCap = "round"
		ctx.lineJoin = "round"
		ctx.lineWidth = 1;
		ctx.strokeStyle = canvas.color;
		ctx.fillStyle = canvas.color;

		ctx.beginPath()

		ctx.moveTo(0, canvas.height);
		let gapX = (canvas.width - canvas.gap)/2
		let ctrX1 = 0.75 * gapX;
		let ctrY1 = 0.75 * canvas.height;
		ctx.quadraticCurveTo(ctrX1 ,ctrY1,(canvas.width - canvas.gap)/2, 0)

		ctx.lineTo((canvas.width + canvas.gap)/2, 0);
		let ctrX2 = canvas.width  - ctrX1;
		let ctrY2 = ctrY1;
		ctx.quadraticCurveTo(ctrX2, ctrY2, canvas.width, canvas.height);

		let concaveCoeff = canvas.isConcave ? 0.75 : 1;
		ctx.quadraticCurveTo(canvas.width/2, concaveCoeff * canvas.height, 0, canvas.height);
		//ctx.lineTo(0, canvas.height);

		ctx.stroke();
		ctx.fill();
	}
}

