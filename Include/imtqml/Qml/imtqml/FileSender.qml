import QtQuick 2.0

QtObject {

    property string state: "";
    property string json;

    signal progress(real loaded, real total);

    function SendFile(fileUrl){
//        console.log("SendFile", fileUrl)
        this.state = "Loading"
        var xhr = new XMLHttpRequest;        

        let reader = new FileReader()
        reader.readAsArrayBuffer(fileUrl)

        reader.onload = ()=>{
//            xhr.open("POST", `../../files?name=${fileUrl.name}`);
            xhr.open("POST", `../../files/${fileUrl.name}`);
            xhr.send(reader.result)
        }

        xhr.onreadystatechange = () => {
            if (xhr.readyState === XMLHttpRequest.DONE){
                this.json = xhr.responseText;
                this.state = "Ready"
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
                this.state = "Done"
            }
        }
    }
}
