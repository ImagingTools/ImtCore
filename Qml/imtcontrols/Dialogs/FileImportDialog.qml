import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import Qt.labs.platform 1.1

FileDialog {
	id: importFileDialog_
	title: qsTr("Import file")
	fileMode: FileDialog.OpenFile
	nameFilters: ["All files (*)"]

	property var mimeTypes: [];
	property bool loadingInParts: false;

	signal importDialogResult(string fileName, string fileNameWithExtension, string encodedData, string mimeType)
	signal beginImport(string fileNameWithExt, string filePath, string fileSize)

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
		let filePath;
		let fileSize = 0;
		if (Qt.platform.os == "web"){
			filePath = importFileDialog_.file.toString()
			fileSize = filePath.size;
		}
		else {
			filePath = importFileDialog_.file.toString()
		}

		filePath = filePath.replace('file:///', '')

		let mimeType = ""
		if (Qt.platform.os == "web"){
			let fileName = filePath.name
			let fileNameWithExt = fileName;
			let data = filePath.name.split('.')
			let ext = "";
			if (data.length > 0){
				ext = data[data.length -1]
				fileName = data[0]
				for(let i = 1; i < (data.length - 1); i++){
					fileName = "." + data[i]
				}
			}
			if (!loadingInParts){
				let reader = new FileReader()

				reader.readAsDataURL(filePath)

				reader.onload = function(){
					let encodedContentWithHeader = reader.result
					let encodedContent = encodedContentWithHeader.replace(/^.{0,}base64,/, '')

					let index = getDialogNameFilterIndex(importFileDialog_.nameFilters, ext)
					if (index < 0){
						console.error("Extension not found in name filters");
						return;
					}

					mimeType = index < mimeTypes.length ? mimeTypes[index] : ""

					let fileNameWithExtension = fileName + "." + ext;

					importDialogResult(fileName, fileNameWithExtension, encodedContent, mimeType);
				}.bind(this)

			}
			else {
				beginImport(fileNameWithExt, filePath, fileSize);
			}
		}//web
		else {
			let fileName = filePath
			let fileNameWithExt = fileName;
			let parts = filePath.split('/')
			if (parts.length > 0){
				fileNameWithExt = parts[parts.length - 1];
				let data = parts[parts.length - 1].split('.')
				let ext = "";
				if (data.length > 0){
					ext = data[data.length - 1]
					fileName = data[0]
					for(let i = 1; i < (data.length - 1); i++){
						fileName = "." + data[i]
					}
				}
			}
			if(!loadingInParts){
				fileIO.source = filePath
				let fileData = fileIO.read()
				fileSize = fileData.length;
				let encodedData = Qt.btoa(fileData);
				let index = getDialogNameFilterIndex(importFileDialog_.nameFilters, ext)
				if (index < 0){
					console.error("Extension not found in namefilters");
					return;
				}
				mimeType = index < mimeTypes.length ? mimeTypes[index] : ""

				let fileNameWithExtension = fileName + "." + ext;

				importDialogResult(fileName, fileNameWithExtension, encodedData, mimeType);
			}
			else {
				//fileSize =
				//beginImport(fileNameWithExt, filePath, fileSize)
			}
		}
	}//onAccepted

	FileIO {
		id: fileIO
	}
}
