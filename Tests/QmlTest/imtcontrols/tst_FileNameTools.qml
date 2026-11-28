import QtQuick 2.12
import QtTest 1.12
import imtcontrols 1.0

// The same cases as CFileNameToolsTest checks for imtfile::CFileNameTools in C++
TestCase {
	id: testCase

	name: "FileNameTools"

	function repeat(text, count){
		var result = ""
		for (var index = 0; index < count; ++index){
			result += text
		}

		return result
	}

	function test_checkFileName_data(){
		var rows = [
			// Valid names
			{tag: "plain", name: "Setup.exe", error: ""},
			{tag: "cyrillic and space", name: "\u041E\u0442\u0447\u0451\u0442 2026.pdf", error: ""},
			{tag: "leading space", name: " leading", error: ""},
			{tag: "leading dot", name: ".hidden", error: ""},
			{tag: "surrogate pair", name: "\uD83D\uDE00.txt", error: ""},
			{tag: "like a device", name: "console.txt", error: ""},
			{tag: "COM10", name: "COM10", error: ""},
			{tag: "LPT", name: "LPT", error: ""},
			{tag: "255 bytes", name: repeat("a", 255), error: ""},
			{tag: "254 bytes of two-byte characters", name: repeat("\u00E9", 127), error: ""},

			// Names that cannot be used
			{tag: "empty", name: "", error: "The name is empty"},
			{tag: "dot", name: ".", error: "The name '.' is not allowed"},
			{tag: "two dots", name: "..", error: "The name '..' is not allowed"},
			{tag: "256 bytes", name: repeat("a", 256), error: "The name is longer than 255 bytes in UTF-8"},
			{tag: "256 bytes of two-byte characters", name: repeat("\u00E9", 128), error: "The name is longer than 255 bytes in UTF-8"},
			{tag: "tab", name: "a\tb", error: "The name contains a control character"},
			{tag: "delete", name: "a\u007Fb", error: "The name contains a control character"},
			{tag: "next line", name: "a\u0085b", error: "The name contains a control character"},
			{tag: "unpaired surrogate", name: "a\uD83Db", error: "The name contains an unpaired surrogate"},
			{tag: "trailing dot", name: "name.", error: "The name ends with a dot or a space"},
			{tag: "trailing space", name: "name ", error: "The name ends with a dot or a space"},
			{tag: "CON", name: "CON", error: "The name 'CON' is reserved for a device on Windows"},
			{tag: "con with an extension", name: "con.txt", error: "The name 'con.txt' is reserved for a device on Windows"},
			{tag: "NUL with two extensions", name: "Nul.tar.gz", error: "The name 'Nul.tar.gz' is reserved for a device on Windows"},
			{tag: "space before the extension", name: "AUX .log", error: "The name 'AUX .log' is reserved for a device on Windows"},
			{tag: "COM1", name: "COM1", error: "The name 'COM1' is reserved for a device on Windows"},
			{tag: "LPT0", name: "lpt0.txt", error: "The name 'lpt0.txt' is reserved for a device on Windows"},
			{tag: "COM superscript", name: "COM\u00B9", error: "The name 'COM\u00B9' is reserved for a device on Windows"}
		]

		var forbiddenCharacters = "<>:\"/\\|?*"
		for (var index = 0; index < forbiddenCharacters.length; ++index){
			var character = forbiddenCharacters.charAt(index)
			rows.push({
				tag: "character " + character,
				name: "a" + character + "b",
				error: "The name contains the character '" + character + "', which is not allowed"
			})
		}

		return rows
	}

	function test_checkFileName(data){
		compare(FileNameTools.checkFileName(data.name), data.error)
	}
}
