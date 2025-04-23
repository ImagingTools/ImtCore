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

	function dateToStr(date, format){
		if(!date || !format) {
			console.log('date and format must be required')
			return ''
		}

		let _date = date

		if(typeof _date === 'number'){
			_date = new Date(date)
		}

		let day = _date.getDate()
		if(day < 10) day = '0'+day
		let month = _date.getMonth()+1
		if(month < 10) month = '0'+month
		let fullYear = _date.getFullYear()
		let year = fullYear.toString().slice(2)

		let seconds = _date.getSeconds()
		if(seconds < 10) seconds  = '0' + seconds
		let minutes = _date.getMinutes()
		if(minutes < 10) minutes = '0' + minutes
		let hours = _date.getHours()
		if(hours < 10) hours = '0' + hours

		return format.replace('yyyy',fullYear).replace('yy',year).replace('MM',month).replace('dd',day).replace('hh',hours).replace('mm',minutes).replace('ss',seconds)
	}


	function formatPhoneNumber(phone, separator){

		phone = phone.replace(" ","");

		let retVal = ""

		let hasPlus = phone[0] == "+"

		if(hasPlus){
			phone = phone.replace("+","");
		}

		let arr = []

		let numberLength = phone.length;

		let count = 0
		let iteration = 0
		let step = 3;

		while(count < numberLength){
			if(iteration == 0){
				arr[0] = phone[0];
				count++
			}
			else {
				if(iteration > 2){
					step = 2;
				}
				let end = Math.min(count + step, numberLength)
				arr[iteration] = phone.slice(count, end)
				count += step;
			}
			iteration++;
		}

		for(let i = 0; i < arr.length; i++){
			let str = String(arr[i])
			if(i !== 0 && str !== ""){
				retVal += separator;
			}
			if(str !== ""){
				retVal += str
			}
		}

		if(hasPlus){
			retVal = "+" + retVal;
		}

		return retVal
	}


	// Supports UTF-8 encoded text. Works in both web and QML (only God knows how)
	function stringToHex(str, delimiter) {
		var utf8 = [];

		for (var i = 0; i < str.length; i++) {
			var code = str.charCodeAt(i);

			// vodoo magic
			if (code < 0x80) {
				utf8.push(code);
			} else if (code < 0x800) {
				utf8.push(0xc0 | (code >> 6));
				utf8.push(0x80 | (code & 0x3f));
			} else if (code < 0xd800 || code >= 0xe000) {
				utf8.push(0xe0 | (code >> 12));
				utf8.push(0x80 | ((code >> 6) & 0x3f));
				utf8.push(0x80 | (code & 0x3f));
			} else {
				// Surrogate pair
				i++;
				code = 0x10000 + (((code & 0x3ff) << 10) | (str.charCodeAt(i) & 0x3ff));
				utf8.push(0xf0 | (code >> 18));
				utf8.push(0x80 | ((code >> 12) & 0x3f));
				utf8.push(0x80 | ((code >> 6) & 0x3f));
				utf8.push(0x80 | (code & 0x3f));
			}
		}

		// Convert bytes to hex
		var hex = [];
		for (var j = 0; j < utf8.length; j++) {
			hex.push(utf8[j].toString(16).padStart(2, '0'));
		}

		return delimiter ? hex.join(delimiter) : hex.join('');
	}

	// Supports UTF-8 encoded text. Works in both web and QML (only God knows how)
	function hexToString(hex, delimiter) {
		var bytes = [];
		var step = delimiter ? 2 + delimiter.length : 2;

		for (var i = 0; i < hex.length; i += step) {
			bytes.push(parseInt(hex.slice(i, i + 2), 16));
		}

		let str = '';
		i = 0;
		while (i < bytes.length) {
			var byte1 = bytes[i++];

			// vodoo magic
			if (byte1 < 0x80) {
				str += String.fromCharCode(byte1);
			} else if (byte1 < 0xe0) {
				let byte2 = bytes[i++];
				str += String.fromCharCode(((byte1 & 0x1f) << 6) | (byte2 & 0x3f));
			} else if (byte1 < 0xf0) {
				let byte2 = bytes[i++];
				let byte3 = bytes[i++];
				str += String.fromCharCode(
					((byte1 & 0x0f) << 12) |
					((byte2 & 0x3f) << 6) |
					(byte3 & 0x3f)
				);
			} else {
				// 4-byte UTF-8 → surrogate pair
				let byte2 = bytes[i++];
				let byte3 = bytes[i++];
				let byte4 = bytes[i++];
				let codepoint = ((byte1 & 0x07) << 18) |
					((byte2 & 0x3f) << 12) |
					((byte3 & 0x3f) << 6) |
					(byte4 & 0x3f);

				codepoint -= 0x10000;
				str += String.fromCharCode(
					0xd800 + (codepoint >> 10),
					0xdc00 + (codepoint & 0x3ff)
				);
			}
		}

		return str;
	}
}
