pragma Singleton
import QtQuick 2.12


QtObject {
	function formatDigitRev(digit){
		return digit.replace(/\s/g,'')
	}


	function roundToPrecision(digit, precision){
		return Math.round(parseFloat(digit) * Math.pow(10,precision)) / Math.pow(10,precision)
	}


	function formatDigit(number_, decimalRoundupPrecision){
		let numberString = String(number_).replace(",",".")
		let number__ =  Number(numberString)

		if (isNaN(number__)){
			console.error("Cannot format invalid number ", number__)

			return number_
		}

		if(decimalRoundupPrecision){
			decimalRoundupPrecision = Math.abs(parseInt(decimalRoundupPrecision, 10))
			let precisionDivider = Math.pow(10, decimalRoundupPrecision)
			number__ = Math.round(number__ * precisionDivider) / precisionDivider
		}

		let splitArr	= number__.toString().split(".")
		let beforeDot	= splitArr[0]
		beforeDot = parseInt(beforeDot, 10).toString()
		let afterDot	= splitArr[1]
		let beforeDotFormatted = "";

		for (let i = 0; i < beforeDot.length; i++){
			if (i > 0 && (beforeDot.length - i) % 3 === 0){
				beforeDotFormatted += " ";
			}
			beforeDotFormatted += beforeDot[i];
		}

		let retVal = beforeDotFormatted
		if (afterDot){
			retVal += ",";
			retVal += !decimalRoundupPrecision ? afterDot : afterDot.padEnd(decimalRoundupPrecision, "0");
		}

		return retVal
	}


	function dateToStr(date, format){
		let defaultFormat = "yyyy-MM-dd hh:mm:ss.zzz"
		if(!date){
			console.error('date must be specified, default format: ' + defaultFormat)

			return ''
		}

		if(!format){
			format = defaultFormat
		}

		let _date = typeof date === 'number' ? new Date(date) : date

		let day = _date.getDate().toString().padStart(2, '0')
		let month = (_date.getMonth() + 1).toString().padStart(2, '0')
		let fullYear = _date.getFullYear()
		let year = fullYear.toString().slice(2)
		let hours = _date.getHours().toString().padStart(2, '0')
		let minutes = _date.getMinutes().toString().padStart(2, '0')
		let seconds = _date.getSeconds().toString().padStart(2, '0')
		let msecs = _date.getMilliseconds()
		let formatted = format
				.replace('yyyy', fullYear)
				.replace('yy', year)
				.replace('MM', month)
				.replace('dd', day)
				.replace('hh', hours)
				.replace('mm', minutes)
				.replace('ss', seconds)

		if (format.includes('.zzz')){
			formatted = formatted.replace('.zzz', '.' + msecs.toString().padStart(3, '0'))
		}
		else if (format.includes('.z')){
			formatted = formatted.replace('.z', '.' + msecs.toString())
		}

		return formatted
	}


	function formatPhoneNumber(phone, separator){
		if(!phone || phone === ""){
			console.error("Cannot format invalid phone number ", phone)

			return
		}
		if(!separator){
			separator = " "
		}

		phone = phone.replace(/\s+/g, "")

		let retVal = ""
		let hasPlus = phone.startsWith("+")
		if(hasPlus){
			phone = phone.slice(1)
		}

		let parts = []
		let index = 1
		parts.push(phone[0])

		while (index < phone.length){
			let step = parts.length > 2 ? 2 : 3;
			parts.push(phone.slice(index, index + step));
			index += step;
		}

		parts = parts.filter(Boolean)
		let formatted = parts.join(separator);

		return hasPlus ? "+" + formatted : formatted;
	}


	// Supports UTF-8 encoded text. Works in both web and QML (only God knows how)
	function stringToHex(str, delimiter){
		let utf8 = [];

		for (let i = 0; i < str.length; i++){
			let code = str.charCodeAt(i);

			// Voodoo magic
			if (code < 0x80){
				utf8.push(code);
			}
			else if (code < 0x800){
				utf8.push(0xc0 | (code >> 6));
				utf8.push(0x80 | (code & 0x3f));
			}
			else if (code < 0xd800 || code >= 0xe000){
				utf8.push(0xe0 | (code >> 12));
				utf8.push(0x80 | ((code >> 6) & 0x3f));
				utf8.push(0x80 | (code & 0x3f));
			}
			else {
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
		for (var j = 0; j < utf8.length; j++){
			hex.push(utf8[j].toString(16).padStart(2, '0'));
		}

		return delimiter ? hex.join(delimiter) : hex.join('');
	}

	// Supports UTF-8 encoded text. Works in both web and QML (only God knows how)
	function hexToString(hex, delimiter){
		let bytes = [];
		let step = delimiter ? 2 + delimiter.length : 2;

		for (let i = 0; i < hex.length; i += step){
			bytes.push(parseInt(hex.slice(i, i + 2), 16));
		}

		let str = '';
		let iter = 0;
		while (iter < bytes.length){
			let byte1 = bytes[iter++];

			// Voodoo magic
			if (byte1 < 0x80){
				str += String.fromCharCode(byte1);
			}
			else if (byte1 < 0xe0){
				let byte2 = bytes[iter++];
				str += String.fromCharCode(((byte1 & 0x1f) << 6) | (byte2 & 0x3f));
			}
			else if (byte1 < 0xf0){
				let byte2 = bytes[iter++];
				let byte3 = bytes[iter++];
				str += String.fromCharCode(
					((byte1 & 0x0f) << 12) |
					((byte2 & 0x3f) << 6) |
					(byte3 & 0x3f)
				);
			}
			else {
				// 4-byte UTF-8 → surrogate pair
				let byte2 = bytes[iter++];
				let byte3 = bytes[iter++];
				let byte4 = bytes[iter++];
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

	function hasElementInList(listArg, valueArg){
		for (let i = 0; i < listArg.length; ++i){
			if (listArg[i] == valueArg){
				return true
			}
		}

		return false
	}

	function lab2rgb(lab){
		let y = (lab[0] + 16) / 116
		let x = lab[1] / 500 + y
		let z = y - lab[2] / 200
		let r
		let g
		let b

		x = 0.95047 * ((x * x * x > 0.008856) ? x * x * x : (x - 16/116) / 7.787);
		y = 1.00000 * ((y * y * y > 0.008856) ? y * y * y : (y - 16/116) / 7.787);
		z = 1.08883 * ((z * z * z > 0.008856) ? z * z * z : (z - 16/116) / 7.787);

		r = x *  3.2406 + y * -1.5372 + z * -0.4986;
		g = x * -0.9689 + y *  1.8758 + z *  0.0415;
		b = x *  0.0557 + y * -0.2040 + z *  1.0570;

		r = (r > 0.0031308) ? (1.055 * Math.pow(r, 1/2.4) - 0.055) : 12.92 * r;
		g = (g > 0.0031308) ? (1.055 * Math.pow(g, 1/2.4) - 0.055) : 12.92 * g;
		b = (b > 0.0031308) ? (1.055 * Math.pow(b, 1/2.4) - 0.055) : 12.92 * b;

		return [Math.max(0, Math.min(1, r)) * 255,
				Math.max(0, Math.min(1, g)) * 255,
				Math.max(0, Math.min(1, b)) * 255]
	}

	function componentToHex(c) {
		let hex = Math.trunc(c).toString(16);
		return hex.length == 1 ? "0" + hex : hex;
	}

	function rgbToHex(r, g, b) {
		return "#" + String(componentToHex(r)) + String(componentToHex(g)) + String(componentToHex(b));
	}

	function labToRgbHex(argL, argA, argB)
	{
		let y = (argL + 16) / 116
		let x = argA / 500 + y
		let z = y - argB / 200
		let r
		let g
		let b

		x = 0.95047 * ((x * x * x > 0.008856) ? x * x * x : (x - 16 / 116) / 7.787)
		y = 1.00000 * ((y * y * y > 0.008856) ? y * y * y : (y - 16 / 116) / 7.787)
		z = 1.08883 * ((z * z * z > 0.008856) ? z * z * z : (z - 16 / 116) / 7.787)

		r = x * 3.2406 + y * -1.5372 + z * -0.4986
		g = x * -0.9689 + y * 1.8758 + z * 0.0415
		b = x * 0.0557 + y * -0.2040 + z * 1.0570

		r = (r > 0.0031308) ? (1.055 * Math.pow(r, 1 / 2.4) - 0.055) : 12.92 * r
		g = (g > 0.0031308) ? (1.055 * Math.pow(g, 1 / 2.4) - 0.055) : 12.92 * g
		b = (b > 0.0031308) ? (1.055 * Math.pow(b, 1 / 2.4) - 0.055) : 12.92 * b

		r = Math.max(0, Math.min(1, r)) * 255
		g = Math.max(0, Math.min(1, g)) * 255
		b = Math.max(0, Math.min(1, b)) * 255

		let rHex = Math.trunc(r).toString(16)
		if (rHex.length == 1) {
			rHex = '0' + rHex
		}

		let gHex = Math.trunc(g).toString(16)
		if (gHex.length == 1) {
			gHex = '0' + gHex
		}
		let bHex = Math.trunc(b).toString(16)
		if (bHex.length == 1) {
			bHex = '0' + bHex
		}

		let retVal = '#' + rHex + gHex + bHex

		return retVal
	}

	function getRadians(degreesArg){
		return degreesArg * Math.PI / 180
	}

	function getAngle(xArg, yArg, center){
		let x0 = center.x
		let y0 = center.y
		let x_ = (xArg- x0)
		let y_ = (yArg - y0)
		let r = Math.sqrt(x_*x_+y_*y_)
		let angle = 0
		if(x_ >= 0 && y_ <= 0)
			angle = Math.asin(Math.abs(x_)/r)*180/Math.PI
		else if(x_ > 0 && y_ > 0)
			angle = Math.acos(Math.abs(x_)/r)*180/Math.PI + 90
		else if(x_ <= 0 && y_ > 0)
			angle = Math.asin(Math.abs(x_)/r)*180/Math.PI + 180
		else if(x_ <= 0 && y_ <= 0)
			angle = Math.acos(Math.abs(x_)/r)*180/Math.PI + 270

		return angle;
	}
}
