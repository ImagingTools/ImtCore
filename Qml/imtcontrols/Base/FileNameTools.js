// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
.pragma library

// Names of files and folders that every common file system takes: NTFS (Windows), ext4 (Linux) and APFS (macOS).
// The same rules and messages as imtfile::CFileNameTools in C++.

// The longest name ext4 and APFS take, in bytes of UTF-8
var maxNameBytes = 255

// The characters NTFS does not take in a name; '/' separates the folders on all of the systems
var forbiddenCharacters = "<>:\"/\\|?*"


function isHighSurrogate(code){
	return code >= 0xD800 && code <= 0xDBFF
}


function isLowSurrogate(code){
	return code >= 0xDC00 && code <= 0xDFFF
}


// The Unicode category Cc, as QChar::Other_Control
function isControlCharacter(code){
	return code <= 0x1F || (code >= 0x7F && code <= 0x9F)
}


function getUtf8Length(text){
	var length = 0
	for (var index = 0; index < text.length; ++index){
		var code = text.charCodeAt(index)
		if (code < 0x80){
			length += 1
		}
		else if (code < 0x800){
			length += 2
		}
		else if (isHighSurrogate(code) && index + 1 < text.length && isLowSurrogate(text.charCodeAt(index + 1))){
			length += 4
			++index
		}
		else{
			length += 3
		}
	}

	return length
}


// A device name of Windows: no file can have it, also with an extension ("CON", "con.txt", "CON .txt")
function isReservedName(name){
	var dotIndex = name.indexOf(".")
	var baseName = dotIndex < 0 ? name : name.substring(0, dotIndex)
	while (baseName.length > 0 && baseName.charAt(baseName.length - 1) === " "){
		baseName = baseName.substring(0, baseName.length - 1)
	}
	baseName = baseName.toUpperCase()

	if (baseName === "CON" || baseName === "PRN" || baseName === "AUX" || baseName === "NUL"){
		return true
	}

	// COM0-COM9 and LPT0-LPT9, with the superscripts 1, 2 and 3 as well
	if (baseName.length === 4 && (baseName.indexOf("COM") === 0 || baseName.indexOf("LPT") === 0)){
		var number = baseName.charAt(3)
		var isDigit = number >= "0" && number <= "9"
		var isSuperscript = number === "\u00B9" || number === "\u00B2" || number === "\u00B3"

		return isDigit || isSuperscript
	}

	return false
}


// Why the name of a file or a folder (one segment of a path) cannot be used; an empty string for a valid name
function checkFileName(name){
	if (!name){
		return "The name is empty"
	}

	if (name === "." || name === ".."){
		return "The name '" + name + "' is not allowed"
	}

	if (getUtf8Length(name) > maxNameBytes){
		return "The name is longer than " + maxNameBytes + " bytes in UTF-8"
	}

	for (var index = 0; index < name.length; ++index){
		var code = name.charCodeAt(index)
		var character = name.charAt(index)
		if (forbiddenCharacters.indexOf(character) >= 0){
			return "The name contains the character '" + character + "', which is not allowed"
		}

		if (isControlCharacter(code)){
			return "The name contains a control character"
		}

		if (isHighSurrogate(code) && index + 1 < name.length && isLowSurrogate(name.charCodeAt(index + 1))){
			++index

			continue
		}

		if (isHighSurrogate(code) || isLowSurrogate(code)){
			return "The name contains an unpaired surrogate"
		}
	}

	var lastCharacter = name.charAt(name.length - 1)
	if (lastCharacter === "." || lastCharacter === " "){
		return "The name ends with a dot or a space"
	}

	if (isReservedName(name)){
		return "The name '" + name + "' is reserved for a device on Windows"
	}

	return ""
}
