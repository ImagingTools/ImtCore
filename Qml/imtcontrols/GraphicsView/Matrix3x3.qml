import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

import imtcontrols 1.0

QtObject {
	id: root

	property var matrix :[
	[1, 0, 0],
	[0, 1, 0],
	[0, 0, 1]
  ];

	function isIdentityMatrix(matrixArg){
		if(matrixArg == undefined){
			matrixArg = matrix;
		}
		return LinearAlgebra.isIdentityMatrix(matrixArg)
	}

	function multiplyByMatrix(matrixArg, externMatrix){
		if(matrixArg == undefined){
			matrixArg = matrix
		}

		return LinearAlgebra.multiplyByMatrix3x3(matrixArg, externMatrix);
	}

	function transformPoint(pointArg, matrixArg){
		if(matrixArg == undefined){
			matrixArg = matrix;
		}
		return LinearAlgebra.transformPoint2d(pointArg, matrixArg)
	}

	function multiplyByNumber(matrixArg, number){
		if(matrixArg == undefined){
			matrixArg = matrix
		}

		return LinearAlgebra.multiplyByNumber(matrixArg, number);
	}

	function getInvertedMatrix(matrixArg){
		if(matrixArg == undefined){
			matrixArg = matrix
		}

		return LinearAlgebra.getInvertedMatrix3x3(matrixArg);
	}

	function getDeterminant(matrixArg){
		if(matrixArg == undefined){
			matrixArg = matrix
		}

		return LinearAlgebra.getDeterminant3x3(matrixArg);
	}

	function getMinorMatrix(matrixArg){
		if(matrixArg == undefined){
			matrixArg = matrix
		}

		return LinearAlgebra.getMinorMatrix3x3(matrixArg);
	}

	function getTransposedMatrix(matrixArg){
		if(matrixArg == undefined){
			matrixArg = matrix
		}

		return LinearAlgebra.getTransposedMatrix3x3(matrixArg);
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

