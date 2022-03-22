import QtQuick 2.0

QtObject {

    property string state: "";
    property string downloadedFileLocation;
    property string downloadedFilePath;
    property string json;    

    signal progress(real bytesLoaded, real bytesTotal);
    signal fileDeleted();
    signal fileDownloaded(string filePath);
    signal fileUploaded();
    signal fileExists();

    function SendFile(fileUrl){
        this.state = "Loading"
        var xhr = new XMLHttpRequest;        

        let reader = new FileReader()
        reader.readAsArrayBuffer(fileUrl)

        xhr.overrideMimeType('text/xml');

        reader.onload = ()=>{
            xhr.open("POST", `../../files/${fileUrl.name}`);
            xhr.send(reader.result)
        }

        xhr.onreadystatechange = () => {
            if (xhr.readyState === xhr.DONE && xhr.status === 409) {
                var existingHash = xhr.responseXML.getElementsByTagName("p")[2].innerHTML;
                this.json = existingHash;
                this.fileExists()
              }
            if (xhr.readyState === XMLHttpRequest.DONE && xhr.status !== 409){
                this.json = xhr.responseText;
                this.state = "Ready"
                this.fileUploaded()
            }
        }
        xhr.onprogress = (event)=>{
            this.progress(event.loaded, event.total)
        }

    }

    function GetFile(fileHash, fileUrl) {
        open(`../../files/${fileUrl.name}?FileId=${fileHash}`)
    }

    function DeleteFile(fileHash){
        this.state = "Loading"
        var xhr = new XMLHttpRequest;
        xhr.open("DELETE", `../../files/${fileHash}`);
        xhr.send(fileHash)

        xhr.onreadystatechange = () => {
            if (xhr.readyState === XMLHttpRequest.DONE){
                this.json = xhr.responseText;
                this.state = "Ready"
                this.fileDeleted()
            }
        }
    }
}
