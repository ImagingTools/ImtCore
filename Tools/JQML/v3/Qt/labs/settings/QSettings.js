const QtObject = require("../../../QtQml/QtObject")
const String = require("../../../QtQml/String")
const Bool = require("../../../QtQml/Bool")
const Signal = require("../../../QtQml/Signal")

class Settings extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {

        category: { type: String, value:''},
        fileName: { type: String, value:''},

        categoryChanged: {type:Signal, args:[]},
        fileNameChanged: {type:Signal, args:[]},

    })

    setValue(key, value){

    }

    sync(){

    }

    value(key, defaultValue){

    }
}



module.exports = Settings