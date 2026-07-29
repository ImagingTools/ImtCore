pragma Singleton
import QtQuick 2.12


QtObject {

	function scalarProduct2d(vector1, vector2){
		return vector1.x * vector2.x + vector1.y * vector2.y
	}

	function vectorLength2d(vectorPoint1, vectorPoint2){
		return distanceBetweenPoints2d(vectorPoint1, vectorPoint2);
	}

	function distanceBetweenPoints2d(point1, point2){
		 return Math.sqrt((point2.x - point1.x)*(point2.x - point1.x) + (point2.y - point1.y)*(point2.y - point1.y))
	}

	function distanceBetweenPointAndLineSection2d(point, linePoint1, linePoint2){
		let withinSection =
			point.x >= Math.min(linePoint1.x, linePoint2.x)
			&& point.x <= Math.max(linePoint1.x, linePoint2.x)
			&& point.y >= Math.min(linePoint1.y, linePoint2.y)
			&& point.y <= Math.max(linePoint1.y, linePoint2.y)

		if(!withinSection){
			return -1;
		}
		return Math.abs((linePoint2.y - linePoint1.y) * point.x  - (linePoint2.x - linePoint1.x) * point.y + linePoint2.x * linePoint1.y - linePoint2.y * linePoint1.x) / distanceBetweenPoints2d(linePoint1, linePoint2)
	}

	function intersectionLines2d(firstLinePoint1, firtsLinePoint2, secondLinePoint1, secondLinePoint2){
		let retPoint = Qt.point(0, 0);

		let denominator = (secondLinePoint2.y - secondLinePoint1.y)(firtsLinePoint2.x - firstLinePoint1.x) -
			(secondLinePoint2.x - secondLinePoint1.x)(firtsLinePoint2.y - firstLinePoint1.y)

		if(denominator == 0){
			return null;
		}

		retPoint.x = (
					(firstLinePoint1.x * firtsLinePoint2.y - firstLinePoint1.y * firtsLinePoint2.x) * (secondLinePoint1.x - secondLinePoint2.x)
					- (firstLinePoint1.x - firtsLinePoint2.x) * (secondLinePoint1.x * secondLinePoint2.y - secondLinePoint1.y * secondLinePoint2.x)
					)/denominator

		retPoint.x = (
					(firstLinePoint1.x * firtsLinePoint2.y - firstLinePoint1.y * firtsLinePoint2.x) * (secondLinePoint1.y - secondLinePoint2.y)
					- (firstLinePoint1.y - firtsLinePoint2.y) * (secondLinePoint1.x * secondLinePoint2.y - secondLinePoint1.y * secondLinePoint2.x)
					)/denominator


		return retPoint
	}

	function intersectionBetweenLineSegments2d(firstSegmentPoint1, firstSegmentPoint2, secondSegmentPoint1, secondSegmentPoint2){
		let retPoint = Qt.point(0, 0);

		let numerator1 = (secondSegmentPoint2.x  - secondSegmentPoint1.x)*(firstSegmentPoint1.y - secondSegmentPoint1.y) -
			(secondSegmentPoint2.y  - secondSegmentPoint1.y)*(firstSegmentPoint1.x - secondSegmentPoint1.x)

		let numerator2 = (firstSegmentPoint2.x  - firstSegmentPoint1.x)*(firstSegmentPoint1.y - secondSegmentPoint1.y) -
			(firstSegmentPoint2.y  - firstSegmentPoint1.y)*(firstSegmentPoint1.x - secondSegmentPoint1.x)

		let denominator = (secondSegmentPoint2.y - secondSegmentPoint1.y)*(firstSegmentPoint2.x - firstSegmentPoint1.x) -
			(secondSegmentPoint2.x - secondSegmentPoint1.x)*(firstSegmentPoint2.y - firstSegmentPoint1.y)

		if(denominator == 0){//the lines are parallel
			return null;
		}

		let u1 =  numerator1 / denominator;
		let u2 =  numerator2 / denominator;

		if(u1 >= 0 && u1 <= 1 && u2 >= 0 && u2 <= 1){
			retPoint.x = secondSegmentPoint1.x + u2 * (secondSegmentPoint2.x - secondSegmentPoint1.x)
			retPoint.y = secondSegmentPoint1.y + u2 * (secondSegmentPoint2.y - secondSegmentPoint1.y)
			return retPoint
		}
		else {
			return null;
		}
	}

	function hasIntersectionBetweenLineSegments2d(firstSegmentPoint1, firstSegmentPoint2, secondSegmentPoint1, secondSegmentPoint2){
		let numerator1 = (secondSegmentPoint2.x  - secondSegmentPoint1.x)*(firstSegmentPoint1.y - secondSegmentPoint1.y) -
			(secondSegmentPoint2.y  - secondSegmentPoint1.y)*(firstSegmentPoint1.x - secondSegmentPoint1.x)

		let numerator2 = (firstSegmentPoint2.x  - firstSegmentPoint1.x)*(firstSegmentPoint1.y - secondSegmentPoint1.y) -
			(firstSegmentPoint2.y  - firstSegmentPoint1.y)*(firstSegmentPoint1.x - secondSegmentPoint1.x)

		let denominator = (secondSegmentPoint2.y - secondSegmentPoint1.y)*(firstSegmentPoint2.x - firstSegmentPoint1.x) -
			(secondSegmentPoint2.x - secondSegmentPoint1.x)*(firstSegmentPoint2.y - firstSegmentPoint1.y)

		if(numerator1 == 0 && numerator2 == 0 && denominator == 0){//the lines coincide
			return true;
		}

		else if(denominator == 0){//the lines are parallel
			return false;
		}

		let u1 =  numerator1 / denominator;
		let u2 =  numerator2 / denominator;

		if(u1 >= 0 && u1 <= 1 && u2 >= 0 && u2 <= 1){
			return true;
		}

		return false
	}

	function pointInPolygon(point, poligonPointList){
		let ok = false;
		if(point == undefined || poligonPointList == undefined || poligonPointList.length < 3){
			return false;
		}

		let intersectionCount = 0;
		for(let i = 0; i < poligonPointList.length; i++){
			let segmentPoint1 = poligonPointList[i]
			let segmentPoint2
			if(i == poligonPointList.length -1){
				segmentPoint2 = poligonPointList[0]
			}
			else {
				segmentPoint2 = poligonPointList[i+1]
			}

			let hasIntersection = hasIntersectionBetweenLineSegments2d(point, Qt.point(100000, point.y), segmentPoint1, segmentPoint2)
			if(hasIntersection){
				intersectionCount++;
			}
		}

		if(intersectionCount && intersectionCount % 2){
			ok = true;
		}
		return ok;
	}

	function nearestPointOnLine2d(point, linePoint1, linePoint2, isSection){
		if(isSection == undefined){
			isSection = false;
		}
		let distanceToPoint1 = distanceBetweenPoints2d(point, linePoint1);
		let distanceToPoint2 = distanceBetweenPoints2d(point, linePoint2);

		let vector_lp1_lp2 = Qt.point(linePoint2.x - linePoint1.x, linePoint2.y - linePoint1.y);
		let vector_lp1_point = Qt.point(point.x - linePoint1.x, point.y - linePoint1.y);

		let lp1_lp2_length = vectorLength2d(linePoint1, linePoint2)
		if(lp1_lp2_length == 0){
			return null
		}

		let coeff = scalarProduct2d(vector_lp1_lp2, vector_lp1_point) / (lp1_lp2_length * lp1_lp2_length)

		let pointOnLine = Qt.point(linePoint1.x + vector_lp1_lp2.x * coeff, linePoint1.y + vector_lp1_lp2.y * coeff)
		let distanceToPointOnLine = distanceBetweenPoints2d(point, pointOnLine);

		let withinSection = !isSection ? true :
										 pointOnLine.x >= Math.min(linePoint1.x, linePoint2.x)
										 && pointOnLine.x <= Math.max(linePoint1.x, linePoint2.x)
										 && pointOnLine.y >= Math.min(linePoint1.y, linePoint2.y)
										 && pointOnLine.y <= Math.max(linePoint1.y, linePoint2.y)


		if(withinSection && distanceToPointOnLine < distanceToPoint1 && distanceToPointOnLine < distanceToPoint2){
			return pointOnLine;
		}
		else if(distanceToPoint1 < distanceToPoint2){
			return linePoint1
		}
		else {
			return linePoint2
		}

	}
}
