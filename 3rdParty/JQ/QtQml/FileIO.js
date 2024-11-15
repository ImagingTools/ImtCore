const QtObject = require("./QtObject")
const String = require("./String")
const Signal = require("./Signal")
const QtFunctions = require('../Qt/functions')

class RemoteFileController extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        source: { type: String, value: '', signalName: 'sourceChanged' },

        sourceChanged: {type:Signal, slotName:'onSourceChanged', args:[]},
    })


    read(){

    }

    write(data){
        let a = document.createElement("a")
        let file = new Blob([data], {type: 'text/plain'})
        a.href = URL.createObjectURL(file)
        a.download = this.source
        a.click()
        a.remove()
    }

    setSource(source){
        this.source = source
    }
}

module.exports = RemoteFileController