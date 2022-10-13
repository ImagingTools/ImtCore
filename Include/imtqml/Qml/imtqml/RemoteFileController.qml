import QtQuick 2.0

QtObject {

    property string state: "";
    property string downloadedFileLocation;
    property string downloadedFilePath;
    property string json;    

    signal progress(real bytesLoaded, real bytesTotal);
    signal fileDeleted(var url);
    signal fileDownloaded(string filePath);
    signal fileUploaded(var url);
    signal fileExists(var url);

    function SendFile(fileUrl){
        this.state = "Loading"
        var xhr = new XMLHttpRequest;        

        let reader = new FileReader()
        reader.readAsArrayBuffer(fileUrl)

        xhr.overrideMimeType('text/xml');

        reader.onload = function(){
            xhr.open("POST", `../../files/${fileUrl.name}`);
            xhr.send(reader.result)
        }.bind(this)

        xhr.onreadystatechange = function(){
            if (xhr.readyState === xhr.DONE && xhr.status === 409) {
                var existingHash = xhr.responseXML.getElementsByTagName("p")[2].innerHTML;
                this.json = existingHash;
                this.fileExists(fileUrl)
                this.fileUploaded(fileUrl)
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

    function GetFile(fileHash, fileUrl) {
        open(`../../files/${fileUrl.name}?FileId=${fileHash}`)
    }

    function DeleteFile(fileHash, fileUrl){
        this.state = "Loading"
        var xhr = new XMLHttpRequest;
        xhr.open("DELETE", `../../files/${fileHash}`);
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
