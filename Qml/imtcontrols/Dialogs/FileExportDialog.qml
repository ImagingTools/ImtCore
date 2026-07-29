import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import Qt.labs.platform 1.1

FileDialog {
	id: fileDialogSave;

	title: qsTr("Save file");
	nameFilters: ["All files (*)"];
	fileMode: FileDialog.SaveFile;
	currentFile: "Default";
	property string fileExt;
	property var mimeTypes: []

	property string dataStr: ""

	function getDialogNameFilterIndex(nameFilters, fileExt){
		for (let i = 0; i < nameFilters.length; i++){
			let nameFilter = nameFilters[i];
			let filterExt = nameFilter.split("*.")[1].split(")")[0];
			if (filterExt === fileExt){
				return i;
			}
		}

		return -1;
	}

	onAccepted: {
		let filePath = fileDialogSave.file.toString();

		let mimeType = ""
		if (Qt.platform.os == "web"){

			exportFileIO.source = fileDialogSave.currentFile;

			let index = fileDialogSave.getDialogNameFilterIndex(fileDialogSave.nameFilters, fileDialogSave.fileExt)
			if (index < 0){
				console.error("Extension not found in name filters");
				return;
			}

			mimeType = mimeTypes[index]
		}
		else{
			let extension = filePath.substring(filePath.lastIndexOf('.') + 1);
			fileDialogSave.fileExt = extension;

			filePath = filePath.replace('file:///', '')
			exportFileIO.source = filePath;

			let index = fileDialogSave.getDialogNameFilterIndex(fileDialogSave.nameFilters, fileDialogSave.fileExt)
			if (index < 0){
				console.error("Extension not found in name filters");
				return;
			}

			mimeType = mimeTypes[index]
		}

		 writeData(fileDialogSave.currentFile)
	}

	function writeData(name){
		if (Qt.platform.os == "web"){
			exportFileIO.source = name + "." + fileExt;
		}

		let encodedStr = Qt.atob(dataStr);
		exportFileIO.write(encodedStr);
	}

	FileIO {
		id: exportFileIO
	}
}
