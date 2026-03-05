const QtObject = require("../QtQml/QtObject")
const String = require("../QtQml/String")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")


class PluginParameter extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        name: {type: String, value:'', },
        value: {type: Var, },

        nameChanged: {type:Signal, args:[]},
        valueChanged: {type:Signal, args:[]},
   
    })

}



module.exports = PluginParameter