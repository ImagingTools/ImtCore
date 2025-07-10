import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {
	id: root

	property var matrix :[
	[1, 0, 0],
	[0, 1, 0],
	[0, 0, 1]
  ];

	function isIdentityMatrix(){
		return (
				matrix[0][0] == 1
				&& matrix[1][1] == 1
				&& matrix[2][2] == 1
				&& matrix[0][1] == 0
				&& matrix[0][2] == 0
				&& matrix[1][0] == 0
				&& matrix[1][2] == 0
				&& matrix[2][0] == 0
				&& matrix[2][1] == 0
					)
	}

	function multiplyByMatrix(matrixArg,externMatrix){
		//console.log("multiplyByMatrix::", matrixArg, externMatrix)
		if(matrixArg == undefined){
			matrixArg = matrix
		}
		if(matrixArg == undefined){
			return;
		}

		let n = matrixArg.length;
		let resultMatrix = [
				[0, 0, 0],
				[0, 0, 0],
				[0, 0, 0]
			  ];
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

	function transformPoint(pointArg, matrix_){
		//vector = [pointArg.x, pointArg.y, 1] - Homogeneous coordinates
		//matrix * vector
		if(matrix_==undefined){
			matrix_ = matrix;
		}
		let retPoint = Qt.point(0, 0);
		retPoint.x = matrix_[0][0] * pointArg.x + matrix_[0][1] * pointArg.y + matrix_[0][2]//* 1
		retPoint.y = matrix_[1][0] * pointArg.x + matrix_[1][1] * pointArg.y + matrix_[1][2]//* 1
		return retPoint;
	}

	function multiplyByNumber(matrixArg, number){
		if(matrixArg == undefined){
			matrixArg = matrix
		}

		for(let i = 0; i < matrixArg.length; i++){
			let row = matrixArg[i]
			for(let j = 0; j < row.length; j++){
				matrixArg[i][j] = number * matrixArg[i][j];
			}
		}

		return matrixArg;
	}

	function getInvertedMatrix(matrixArg){
		if(matrixArg == undefined){
			matrixArg = matrix
		}

		let determinant = getDeterminant(matrixArg);

		let minorMatrix = getMinorMatrix(matrixArg);

		minorMatrix[0][1] = -minorMatrix[0][1];
		minorMatrix[1][0] = -minorMatrix[1][0];
		minorMatrix[1][2] = -minorMatrix[1][2];
		minorMatrix[2][1] = -minorMatrix[2][1];

		let minorMatrixTransp = getTransposedMatrix(minorMatrix);

		let invertedMatrix = multiplyByNumber(minorMatrixTransp, 1/determinant)

		return invertedMatrix;
	}

	function getDeterminant(matrixArg){
		if(matrixArg == undefined){
			matrixArg = matrix
		}

		let det =
			matrixArg[0][0] * matrixArg[1][1] * matrixArg[2][2]
			- matrixArg[0][0] * matrixArg[1][2] * matrixArg[2][1]
			- matrixArg[0][1] * matrixArg[1][0] * matrixArg[2][2]
			+ matrixArg[0][1] * matrixArg[1][2] * matrixArg[2][0]
			+ matrixArg[0][2] * matrixArg[1][0] * matrixArg[2][1]
			- matrixArg[0][2] * matrixArg[1][1] * matrixArg[2][0]

		return det;
	}

	function getMinorMatrix(matrixArg){
		if(matrixArg == undefined){
			matrixArg = matrix
		}

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

	function getTransposedMatrix(matrixArg){
		if(matrixArg == undefined){
			matrixArg = matrix
		}

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

	function copyFrom(externMatrix){
		matrix = externMatrix;
	}

	function  reset(){
		let matrix1 = [
				[1, 0, 0],
				[0, 1, 0],
				[0, 0, 1]
			  ];

		matrix = matrix1;
	}
}

