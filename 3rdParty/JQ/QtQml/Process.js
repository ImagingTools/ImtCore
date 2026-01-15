const QtObject = require("./QtObject")
const String = require("./String")
const Signal = require("./Signal")

class Process extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        finished: {type:Signal, args:[]},
    })


    start(...args){}
    setEnviroment(...args){}
}



module.exports = Process