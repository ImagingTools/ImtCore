const { QtObject } = require('./QtObject')
const { QString } = require('../utils/properties')

class FileIO extends QtObject {

    static defaultProperties = {
        source: { type: QString, value: '', changed: '$sourceChanged' },
    }

    static defaultSignals = {

    }

    constructor(parent,exCtx,exModel) {
        super(parent,exCtx,exModel)

    }

    read(){

    }

    write(data){
        let byteNumbers = new Array(data.length)
        for (let i = 0; i < data.length; i++) {
            byteNumbers[i] = data.charCodeAt(i)
        }

        let byteArray = new Uint8Array(byteNumbers)

        let a = document.createElement("a")
        let file = new Blob([byteArray])
        a.href = URL.createObjectURL(file)
        a.download = this.getPropertyValue('source')
        a.click()
        a.remove()
    }

    setSource(source){
        this.getProperty('source').reset(source)
    }

    $sourceChanged(){

    }
}

module.exports.FileIO = FileIO