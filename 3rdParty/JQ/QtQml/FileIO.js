const QtObject = require("./QtObject")
const String = require("./String")
const Signal = require("./Signal")
const QtFunctions = require('../Qt/functions')

class FileIO extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        source: { type: String, value: '', signalName: 'sourceChanged' },

        sourceChanged: {type:Signal, slotName:'onSourceChanged', args:[]},
    })


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
        a.download = this.source
        a.click()
        a.remove()
    }

    setSource(source){
        this.source = source
    }
}



module.exports = FileIO