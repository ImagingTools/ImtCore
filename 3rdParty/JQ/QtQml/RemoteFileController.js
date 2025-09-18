const QtObject = require("./QtObject")
const String = require("./String")
const Signal = require("./Signal")
const QtFunctions = require('../Qt/functions')

class RemoteFileController extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        json: { type: String, value: ''},
        state: { type: String, value: ''},
        downloadedFileLocation: { type: String, value: ''},
        downloadedFilePath: { type: String, value: ''},
        prefix: { type: String, value: ''},

        jsonChanged: {type:Signal, args:[]},
        stateChanged: {type:Signal, args:[]},
        downloadedFileLocationChanged: {type:Signal, args:[]},
        downloadedFilePathChanged: {type:Signal, args:[]},
        prefixChanged: {type:Signal, args:[]},

        progress: {type:Signal, args:['bytesLoaded', 'bytesTotal']},
        fileDeleted: {type:Signal, args:['url']},
        fileDownloaded: {type:Signal, args:['filePath']},
        fileUploaded: {type:Signal, args:['url']},
        fileExists: {type:Signal, args:['url']},
        fileDownloadFailed: {type:Signal, args:[]},
        fileUploadFailed: {type:Signal, args:[]},
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