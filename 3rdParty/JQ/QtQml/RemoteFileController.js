const QtObject = require("./QtObject")
const String = require("./String")
const Signal = require("./Signal")
const QtFunctions = require('../Qt/functions')

class RemoteFileController extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        json: { type: String, value: '', signalName: 'jsonChanged' },
        state: { type: String, value: '', signalName: 'stateChanged' },
        downloadedFileLocation: { type: String, value: '', signalName: 'downloadedFileLocationChanged' },
        downloadedFilePath: { type: String, value: '', signalName: 'downloadedFilePathChanged' },
        prefix: { type: String, value: '', signalName: 'prefixChanged' },

        jsonChanged: {type:Signal, slotName:'onJsonChanged', args:[]},
        stateChanged: {type:Signal, slotName:'onStateChanged', args:[]},
        downloadedFileLocationChanged: {type:Signal, slotName:'onDownloadedFileLocationChanged', args:[]},
        downloadedFilePathChanged: {type:Signal, slotName:'onDownloadedFilePathChanged', args:[]},
        prefixChanged: {type:Signal, slotName:'onPrefixChanged', args:[]},

        progress: {type:Signal, slotName:'onProgress', args:['bytesLoaded', 'bytesTotal']},
        fileDeleted: {type:Signal, slotName:'onFileDeleted', args:['url']},
        fileDownloaded: {type:Signal, slotName:'onFileDownloaded', args:['filePath']},
        fileUploaded: {type:Signal, slotName:'onFileUploaded', args:['url']},
        fileExists: {type:Signal, slotName:'onFileExists', args:['url']},
        fileDownloadFailed: {type:Signal, slotName:'onFileDownloadFailed', args:[]},
        fileUploadFailed: {type:Signal, slotName:'onFileUploadFailed', args:[]},
    })


    SendFile(fileUrl){
        this.state = "Loading"
        var xhr = new XMLHttpRequest()     

        let reader = new FileReader()
        reader.readAsArrayBuffer(fileUrl)

        xhr.overrideMimeType('text/xml')

        reader.onload = ()=>{
            if (this.prefix == ""){
                this.prefix = "files"
            }
            let pathIn = `../../` + this.prefix + `/${fileUrl.name}`
            pathIn = pathIn.replace('\/\/','/')
            xhr.open("POST", pathIn)
            xhr.send(reader.result)
        }

        xhr.onreadystatechange = ()=>{
            if (xhr.readyState === xhr.DONE && xhr.status === 409) {
                var existingHash = xhr.responseXML.getElementsByTagName("p")[2].innerHTML
                this.json = existingHash
                this.fileExists(fileUrl)
            }
            if (xhr.readyState === XMLHttpRequest.DONE && xhr.status !== 409){
                this.json = xhr.responseText
                this.state = "Ready"
                this.fileUploaded(fileUrl)
            }
        }
        
        xhr.onprogress = (event)=>{
            this.progress(event.loaded, event.total)
        }

    }

    GetFile(fileHash, fileName) {
        if (this.prefix == ""){
            this.prefix = "files"
        }
        let pathIn = `../../` + this.prefix + `/${fileName}?FileId=${fileHash}`
        pathIn = pathIn.replace('\/\/','/')
        QtFunctions.openUrlExternally(pathIn)
    }

    DeleteFile(fileHash, fileUrl){
        if (this.prefix == ""){
            this.prefix = "files"
        }
        this.state = "Loading"
        var xhr = new XMLHttpRequest()
        let pathIn = `../../` + this.prefix + `/${fileHash}`
        pathIn = pathIn.replace('\/\/','/')
        xhr.open("DELETE", pathIn)
        xhr.send(fileHash)

        xhr.onreadystatechange = ()=>{
            if (xhr.readyState === XMLHttpRequest.DONE){
                this.json = xhr.responseText
                this.state = "Ready"
                this.fileDeleted(fileUrl)
            }
        }
    }
}



module.exports = RemoteFileController