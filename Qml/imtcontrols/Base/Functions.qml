pragma Singleton
import QtQuick 2.12

QtObject {

	//operations with numbers

	function formatDigit(digit){

		var digitString = String(digit).replace(",",".")
		var digit_ = Number(digitString)

		var beforeDot = Math.floor(digit_)
		var afterDot = digitString.match(/\.\d{0,}/) === null ? "" : digitString.match(/\.\d{0,}/)[0]
		if(afterDot === ".")
			afterDot = ""
		//console.log(afterDot)
		var currVal = beforeDot
		var arr = []
		var count = 0

		var retVal = ""

		var str

		while(currVal >= 1000){
			if(currVal / 1000 >= 1){
				arr[count] = String(currVal).substring(String(currVal).length - 3)
				currVal = Math.floor(currVal / 1000)
				count++
			}

		}

		retVal = currVal

		for(var i = arr.length - 1; i >= 0; i--){
			//console.log(arr[i])
			retVal += " "
			retVal += String(arr[i])

		}

		//округление до сотых
//        if(afterDot !== "")
//            afterDot = String(Math.round(parseFloat(afterDot) * 100) / 100).match(/\.\d{0,}/)[0]


		retVal += afterDot

		retVal = retVal.replace(".", ",")

		//console.log("FORMAT_DIGIT: " + retVal)
		return retVal
	}

	function formatDigitRev(digit){
		return digit.replace(/\s/g,'')
	}

	function roundToPrecision(digit, precision){
		return Math.round(parseFloat(digit) * Math.pow(10,precision)) / Math.pow(10,precision)
	}
}
