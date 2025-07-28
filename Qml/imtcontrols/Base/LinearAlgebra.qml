pragma Singleton
import QtQuick 2.12


QtObject {

	// matrix :[
	// [1, 0, 0],
	// [0, 1, 0],
	// [0, 0, 1]
 //  ];

	function transformPoint2d(pointArg, matrixArg){
		//vector = [pointArg.x, pointArg.y, 1] - Homogeneous coordinates
		//matrix * vector

		let retPoint = Qt.point(0, 0);
		retPoint.x = matrixArg[0][0] * pointArg.x + matrixArg[0][1] * pointArg.y + matrixArg[0][2]//* 1
		retPoint.y = matrixArg[1][0] * pointArg.x + matrixArg[1][1] * pointArg.y + matrixArg[1][2]//* 1
		return retPoint;
	}

	function rotatePoint(angle, center, pointArg){
		if(center == undefined){
			center = Qt.point(0,0);
		}

		let matrix;

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
			matrix = multiplyByMatrix3x3(rotationMatrix, translateMatrix)
		}

		return transformPoint2d(pointArg, matrix)
	}

	function multiplyByMatrix3x3(matrixArg, externMatrix){
		let resultMatrix = [
				[0, 0, 0],
				[0, 0, 0],
				[0, 0, 0]
			  ];

		if(matrixArg == undefined || externMatrix == undefined){
			return resultMatrix;
		}

		let n = matrixArg.length;

		if(matrixArg.length !== externMatrix.length){
			return;
		}
		for(let i = 0; i < n; i++){
			let row = matrixArg[i]
			for(let j = 0; j < n; j++){
				resultMatrix[i][j] = 0;
				for(let k = 0; k < n; k++){
					resultMatrix[i][j] += matrixArg[i][k] * externMatrix[k][j];
				}
			}
		}
		return resultMatrix;
	}

	function multiplyByNumber(matrixArg, number){
		for(let i = 0; i < matrixArg.length; i++){
			let row = matrixArg[i]
			for(let j = 0; j < row.length; j++){
				matrixArg[i][j] = number * matrixArg[i][j];
			}
		}

		return matrixArg;
	}

	function isIdentityMatrix(matrixArg){
		for(let i = 0; i < matrixArg.length; i++){
			let row = matrixArg[i]
			for(let j = 0; j < row.length; j++){
				if(i == j && matrixArg[i][j] !==1){
					return false;
				}
				if(i !== j && matrixArg[i][j] !==0){
					return false;
				}
			}
		}

		return true;
	}

	function getInvertedMatrix3x3(matrixArg){
		let determinant = getDeterminant3x3(matrixArg);

		let minorMatrix = getMinorMatrix3x3(matrixArg);

		minorMatrix[0][1] = -minorMatrix[0][1];
		minorMatrix[1][0] = -minorMatrix[1][0];
		minorMatrix[1][2] = -minorMatrix[1][2];
		minorMatrix[2][1] = -minorMatrix[2][1];

		let minorMatrixTransp = getTransposedMatrix3x3(minorMatrix);

		let invertedMatrix = multiplyByNumber(minorMatrixTransp, 1/determinant)

		return invertedMatrix;
	}

	function getDeterminant3x3(matrixArg){
		let det =
			matrixArg[0][0] * matrixArg[1][1] * matrixArg[2][2]
			- matrixArg[0][0] * matrixArg[1][2] * matrixArg[2][1]
			- matrixArg[0][1] * matrixArg[1][0] * matrixArg[2][2]
			+ matrixArg[0][1] * matrixArg[1][2] * matrixArg[2][0]
			+ matrixArg[0][2] * matrixArg[1][0] * matrixArg[2][1]
			- matrixArg[0][2] * matrixArg[1][1] * matrixArg[2][0]

		return det;
	}

	function getMinorMatrix3x3(matrixArg){
		let minorMatrix = [
				[0, 0, 0],
				[0, 0, 0],
				[0, 0, 0]
			  ];

		minorMatrix[0][0] = matrixArg[1][1] * matrixArg[2][2] - matrixArg[1][2] * matrixArg[2][1]
		minorMatrix[0][1] = matrixArg[1][0] * matrixArg[2][2] - matrixArg[1][2] * matrixArg[2][0]
		minorMatrix[0][2] = matrixArg[1][0] * matrixArg[2][1] - matrixArg[2][2] * matrixArg[2][0]

		minorMatrix[1][0] = matrixArg[0][1] * matrixArg[2][2] - matrixArg[0][2] * matrixArg[2][1]
		minorMatrix[1][1] = matrixArg[0][0] * matrixArg[2][2] - matrixArg[0][2] * matrixArg[2][0]
		minorMatrix[1][2] = matrixArg[0][0] * matrixArg[2][1] - matrixArg[0][1] * matrixArg[2][0]

		minorMatrix[2][0] = matrixArg[0][1] * matrixArg[1][2] - matrixArg[0][2] * matrixArg[1][1]
		minorMatrix[2][1] = matrixArg[0][0] * matrixArg[1][2] - matrixArg[0][2] * matrixArg[1][0]
		minorMatrix[2][2] = matrixArg[0][0] * matrixArg[1][1] - matrixArg[0][1] * matrixArg[1][0]

		return minorMatrix;
	}

	function getTransposedMatrix3x3(matrixArg){
		let transposedMatrix = [
				[0, 0, 0],
				[0, 0, 0],
				[0, 0, 0]
			  ];

		for(let i = 0; i < matrixArg.length; i++){
			let row = matrixArg[i]
			for(let j = 0; j < row.length; j++){
				transposedMatrix[i][j] = matrixArg[j][i];
			}
		}

		return transposedMatrix;
	}

	function getDeterminant2x2(matrixArg){
		let det =
			matrixArg[0][0] * matrixArg[1][1]
			- matrixArg[0][1] * matrixArg[1][0]

		return det;
	}
}
