import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {
	id: canvasMatrix

	property real xScale: 1				// a
	property real ySkew: 0				// b
	property real xSkew: 0				// c
	property real yScale: 1				// d
	property real xTranslation: 0		// e
	property real yTranslation: 0		// f


	// matrix:
	// a	c	e
	// b	d	f
	// 0	0	1

	Component.onCompleted: {
		saveInitialData()
	}

	function setContextTransform(ctx){
		ctx.setTransform(
					canvasMatrix.xScale,
					canvasMatrix.ySkew,
					canvasMatrix.xSkew,
					canvasMatrix.yScale,
					canvasMatrix.xTranslation,
					canvasMatrix.yTranslation
					);
	}

	function transformContext(ctx){
		ctx.transform(
					canvasMatrix.xScale,
					canvasMatrix.ySkew,
					canvasMatrix.xSkew,
					canvasMatrix.yScale,
					canvasMatrix.xTranslation,
					canvasMatrix.yTranslation
					);
	}



	//	Rotation matrix:
	//	cos(angle)	-sin(angle)		0
	//	sin(angle)	cos(angle)		0
	//	0			0				1

	function rotateContext(ctx, angle, center){
		if(center == undefined){
			center = Qt.point(0,0);
		}
		xTranslation = center.x
		yTranslation = center.y
		xScale = Math.cos(angle)
		yScale = Math.cos(angle)
		xSkew = -1*Math.sin(angle)
		ySkew = Math.sin(angle)

		setContextTransform(ctx);

		ctx.translate(-center.x, -center.y)
	}

	function  reset(ctx){
		xScale = 1
		ySkew = 0
		xSkew = 0
		yScale = 1
		xTranslation = 0
		yTranslation = 0
	}

	function restore(){
		xScale = _xScaleInitial
		ySkew = _ySkewInitial
		xSkew = _xSkewInitial
		yScale = _yScaleInitial
		xTranslation = _xTranslationInitial
		yTranslation = _yTranslationInitial
	}

	function saveInitialData(){
		_xScaleInitial = xScale
		_ySkewInitial = ySkew
		_xSkewInitial = xSkew
		_yScaleInitial = yScale
		_xTranslationInitial = xTranslation
		_yTranslationInitial = yTranslation
	}

	property real _xScaleInitial: 1
	property real _ySkewInitial: 0
	property real _xSkewInitial: 0
	property real _yScaleInitial: 1
	property real _xTranslationInitial: 0
	property real _yTranslationInitial: 0
}

