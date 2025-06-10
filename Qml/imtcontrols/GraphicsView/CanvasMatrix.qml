import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

Matrix3x3 {
	id: canvasMatrix

	// matrix:
	// a	c	e			xScale	xSkew	xTranslation
	// b	d	f			ySkew	yScale	yTranslation
	// 0	0	1			0		0		1

	//getters
	function xScale(){
		if(matrix == undefined){
			return 1
		}
		return matrix[0][0]
	}

	function ySkew(){
		if(matrix == undefined){
			return 0
		}
		return matrix[1][0]
	}

	function xSkew(){
		if(matrix == undefined){
			return 0
		}
		return matrix[0][1]
	}

	function yScale(){
		if(matrix == undefined){
			return 1
		}
		return matrix[1][1]
	}

	function xTranslation(){
		if(matrix == undefined){
			return 0
		}
		return matrix[0][2]
	}

	function yTranslation(){
		if(matrix == undefined){
			return 0
		}
		return matrix[1][2]
	}

	//setters
	function setXScale(xScaleArg){
		matrix[0][0] = xScaleArg
	}

	function setYSkew(ySkewArg){
		matrix[1][0] = ySkewArg
	}

	function setXSkew(xSkewArg){
		matrix[0][1] = xSkewArg
	}

	function setYScale(yScaleArg){
		matrix[1][1] = yScaleArg
	}

	function setXTranslation(xTranslationArg){
		matrix[0][2] = xTranslationArg
	}

	function setYTranslation(yTranslationArg){
		matrix[1][2] = yTranslationArg
	}

	function setContextTransform(ctx){
		ctx.setTransform(
					canvasMatrix.xScale(),
					canvasMatrix.ySkew(),
					canvasMatrix.xSkew(),
					canvasMatrix.yScale(),
					canvasMatrix.xTranslation(),
					canvasMatrix.yTranslation()
					);
	}

	function transformContext(ctx){
		ctx.transform(
					canvasMatrix.xScale(),
					canvasMatrix.ySkew(),
					canvasMatrix.xSkew(),
					canvasMatrix.yScale(),
					canvasMatrix.xTranslation(),
					canvasMatrix.yTranslation()
					);
	}

	//	Rotation matrix:
	//	cos(angle)	-sin(angle)		0
	//	sin(angle)	cos(angle)		0
	//	0			0				1
	function rotateContext(ctx, angle, center){
		//console.log("rotateContext")
		if(center == undefined){
			center = Qt.point(0,0);
		}

		let rotationMatrix = [
				[Math.cos(angle), -1*Math.sin(angle), center.x],
				[Math.sin(angle), Math.cos(angle), center.y],
				[0, 0, 1]
			  ];
		let translateMatrix = [
				[1, 0, -center.x],
				[0, 1, -center.y],
				[0, 0, 1]
			  ];

		if(center.x == 0 && center.y == 0){
			matrix = rotationMatrix
		}
		else {
			matrix = multiplyByMatrix(rotationMatrix, translateMatrix)
		}
		setContextTransform(ctx);
	}

	function translateContext(ctx, xArg, yArg){
		//console.log("translateContext")
		let matrix1 = [
				[1, 0, 0],
				[0, 1, 0],
				[0, 0, 1]
			  ];
		let translateMatrix = [
				[1, 0, xArg],
				[0, 1, yArg],
				[0, 0, 1]
			  ];
		matrix = multiplyByMatrix(matrix1, translateMatrix)
		setContextTransform(ctx);
	}

	function restore(){
		setXScale(_xScaleInitial)
		setYSkew(_ySkewInitial)
		setXSkew(_xSkewInitial)
		setYScale(_yScaleInitial)
		setXTranslation(_xTranslationInitial)
		setYTranslation(_yTranslationInitial)
	}

	function saveInitialData(){
		_xScaleInitial = xScale()
		_ySkewInitial = ySkew()
		_xSkewInitial = xSkew()
		_yScaleInitial = yScale()
		_xTranslationInitial = xTranslation()
		_yTranslationInitial = yTranslation()
	}

	property real _xScaleInitial: 1
	property real _ySkewInitial: 0
	property real _xSkewInitial: 0
	property real _yScaleInitial: 1
	property real _xTranslationInitial: 0
	property real _yTranslationInitial: 0
}

