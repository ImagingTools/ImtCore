import QtQuick 2.12


/**
	\brief draws a pointed polyline with a gradient between points
	\note a color is defined for each point
*/
GraphicsShapeBase {
	id: polylineGridShape

	/**
		\brief an array of points to draw
		\details points MUST be provided as array of 'points' each 'point' MUST contain 'x','y','color' i.e. {'x': 33, 'y': 11, 'color': 'green'}
					params x,y - is a number (real) param 'color' - is acceptable color type, may be a text or HEX value
		\example [{'x': 50, 'y': 40, 'color': 'red'}, {'x': 10, 'y': 20, 'color': '#F0F'}]
	*/
	property var points: []

	/**
		\brief size of line's point
		\note if size less or equal 0 - points will not be drawn
	*/
	property real pointSize: 5

	/**
		\brief size of line's point
		\warning MUST be greater 0
	*/
	property real lineWidth: 3

	// reimplmented (GraphicsShapeBase)
	function draw(ctx, layerId){
		if (!polylineGridShape.points || polylineGridShape.points.length < 2){
			console.warning('GradientPolyLine: draw: not enough points to draw a line', polylineGridShape.points.length, polylineGridShape.points)

			return
		}

		if (polylineGridShape.lineWidth <= 0){
			console.assert('Unable to draw line if width is less or equal 0', polylineGridShape.lineWidth)

			return
		}

		ctx.save()

		ctx.lineWidth = polylineGridShape.lineWidth

		// initial setup
		ctx.lineJoin = 'round'
		ctx.lineCap = 'round'

		// move to first position
		const firstPoint = polylineGridShape.points[0]
		polylineGridShape.drawPoint(ctx, firstPoint.x, firstPoint.y, firstPoint.color, true)

		ctx.beginPath()
		ctx.moveTo(firstPoint.x, firstPoint.y)

		// draw lines for middle points
		const pointCount = polylineGridShape.points.length
		for (let i = 1; i < pointCount - 1; i++) {
			const prevPoint = polylineGridShape.points[i-1]
			const currentPoint = polylineGridShape.points[i]
			const contextGradient = ctx.createLinearGradient(prevPoint.x, prevPoint.y, currentPoint.x, currentPoint.y)
			contextGradient.addColorStop(0, prevPoint.color)
			contextGradient.addColorStop(1, currentPoint.color)

			ctx.strokeStyle = contextGradient
			ctx.lineTo(currentPoint.x, currentPoint.y)
			ctx.stroke()

			polylineGridShape.drawPoint(ctx, currentPoint.x, currentPoint.y, currentPoint.color, true)

			ctx.beginPath()
			ctx.moveTo(currentPoint.x, currentPoint.y)
		}

		// finalize polyline
		const preLastPoint = polylineGridShape.points[pointCount - 2]
		const lastPoint = polylineGridShape.points[pointCount - 1]
		const contextGradient = ctx.createLinearGradient(preLastPoint.x, preLastPoint.y, lastPoint.x, lastPoint.y)
		contextGradient.addColorStop(0, preLastPoint.color)
		contextGradient.addColorStop(1, lastPoint.color)
		
		ctx.strokeStyle = contextGradient
		ctx.lineTo(lastPoint.x, lastPoint.y)
		ctx.stroke()

		polylineGridShape.drawPoint(ctx, lastPoint.x, lastPoint.y, lastPoint.color, true)

		ctx.restore()
	}

	// private functions

	/// draws a point of the line
	function drawPoint(ctx, x, y, aColor, fillCircle){
		if (polylineGridShape.pointSize <= 0){
			return
		}

		ctx.save()

		if (aColor){
			if (fillCircle){
				ctx.fillStyle = aColor
			}
			else {
				ctx.strokeStyle = aColor
			}
		}

		ctx.beginPath()
		ctx.arc(x, y, polylineGridShape.pointSize, 0, Math.PI * 2)
		if (fillCircle){
			ctx.fill()
		}
		else {
			ctx.stroke()
		}

		ctx.restore()
	}
}

