import QtQuick 2.0

QtObject {

    property string state: "";
    property string downloadedFileLocation;
    property string downloadedFilePath;
    property string json;    
    property string prefix;

    signal progress(real bytesLoaded, real bytesTotal);
    signal fileDeleted(var url);
    signal fileDownloaded(string filePath);
    signal fileUploaded(var url);
    signal fileExists(var url);
    signal fileDownloadFailed();
    signal fileUploadFailed();


    function SendFile(fileUrl){
        this.state = "Loading"
        var xhr = new XMLHttpRequest;        

        let reader = new FileReader()
        reader.readAsArrayBuffer(fileUrl)

        xhr.overrideMimeType('text/xml');

        reader.onload = function(){
            if (this.prefix == ""){
                this.prefix = "files"
            }
            let pathIn = `../../` + this.prefix + `/${fileUrl.name}`;
            pathIn = pathIn.replace('\/\/','/')
            xhr.open("POST", pathIn);
            xhr.send(reader.result)
        }.bind(this)

        xhr.onreadystatechange = function(){
            if (xhr.readyState === xhr.DONE && xhr.status === 409) {
                var existingHash = xhr.responseXML.getElementsByTagName("p")[2].innerHTML;
                this.json = existingHash;
                this.fileExists(fileUrl)
//                this.fileUploaded(fileUrl)
              }
            if (xhr.readyState === XMLHttpRequest.DONE && xhr.status !== 409){
                this.json = xhr.responseText;
                this.state = "Ready"
                this.fileUploaded(fileUrl)
            }
        }.bind(this)
        xhr.onprogress = function(event){
            this.progress(event.loaded, event.total)
        }.bind(this)

    }

    function GetFile(fileHash, fileName) {
        if (this.prefix == ""){
            this.prefix = "files"
        }
        let pathIn = `../../` + this.prefix + `/${fileName}?FileId=${fileHash}`;
        pathIn = pathIn.replace('\/\/','/')
        Qt.openUrlExternally(pathIn)
    }

    function DeleteFile(fileHash, fileUrl){
        if (this.prefix == ""){
            this.prefix = "files"
        }
        this.state = "Loading"
        var xhr = new XMLHttpRequest;
        let pathIn = `../../` + this.prefix + `/${fileHash}`;
        pathIn = pathIn.replace('\/\/','/')
        xhr.open("DELETE", pathIn);
        xhr.send(fileHash)

        xhr.onreadystatechange = function(){
            if (xhr.readyState === XMLHttpRequest.DONE){
                this.json = xhr.responseText;
                this.state = "Ready"
                this.fileDeleted(fileUrl)
            }
        }.bind(this)
    }
}
