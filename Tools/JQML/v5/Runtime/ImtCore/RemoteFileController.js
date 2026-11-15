const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

// ---- RemoteFileController (ImtCore), as the current web runtime does it ---------------------------

class RemoteFileController extends QtObject {
    $path(rest){
        let prefix = this.$v[RemoteFileController.PREFIX] || 'files'
        return ('../../' + prefix + '/' + rest).replace(/([^:])\/\//g, '$1/')
    }

    sendFile(file){
        this.state = 'Loading'
        let xhr = new XMLHttpRequest()
        let reader = new FileReader()
        reader.onload = () => {
            xhr.open('POST', this.$path(file.name))
            xhr.send(reader.result)
        }
        xhr.onreadystatechange = () => {
            if(xhr.readyState !== 4 || this.$f & engine.DESTROYED) return
            engine.batch(() => {
                if(xhr.status === 409){
                    let doc = xhr.responseXML
                    let p = doc ? doc.getElementsByTagName('p')[2] : null
                    this.json = p ? p.innerHTML : xhr.responseText
                    engine.emitArgs(this, RemoteFileController.SIGNAL_fileExists, [file])
                } else if(xhr.status >= 200 && xhr.status < 300){
                    this.json = xhr.responseText
                    this.state = 'Ready'
                    engine.emitArgs(this, RemoteFileController.SIGNAL_fileUploaded, [file])
                } else {
                    this.state = 'Error'
                    engine.emitArgs(this, RemoteFileController.SIGNAL_fileUploadFailed, [])
                }
            })
        }
        xhr.upload.onprogress = (event) => engine.batch(() => engine.emitArgs(this, RemoteFileController.SIGNAL_progress, [event.loaded, event.total]))
        xhr.overrideMimeType('text/xml')
        reader.readAsArrayBuffer(file)
        return true
    }

    getFile(fileId, fileName){
        if(typeof window !== 'undefined') window.open(this.$path(fileName + '?FileId=' + fileId), '_blank')
        return true
    }

    deleteFile(fileId, fileUrl){
        this.state = 'Loading'
        let xhr = new XMLHttpRequest()
        xhr.open('DELETE', this.$path(fileId))
        xhr.onreadystatechange = () => {
            if(xhr.readyState !== 4 || this.$f & engine.DESTROYED) return
            engine.batch(() => {
                this.json = xhr.responseText
                this.state = 'Ready'
                engine.emitArgs(this, RemoteFileController.SIGNAL_fileDeleted, [fileUrl])
            })
        }
        xhr.send(fileId)
        return true
    }
}
defineType(RemoteFileController, {
    props: { json: 'string', state: 'string', downloadedFileLocation: 'string', downloadedFilePath: 'string', prefix: 'string' },
    signals: ['progress(bytesLoaded, bytesTotal)', 'fileDeleted(url)', 'fileDownloaded(filePath)', 'fileUploaded(url)',
        'fileExists(url)', 'fileDownloadFailed', 'fileUploadFailed'],
})

module.exports = { RemoteFileController }
