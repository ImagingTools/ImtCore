const QtObject = require("../QtQml/QtObject")
const String = require("../QtQml/String")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")


class Plugin extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        name: {type: String, value:'osm', },
        parameters: {type: Var, },

        nameChanged: {type:Signal, args:[]},
        parametersChanged: {type:Signal, args:[]},
   
    })

    parameters = []
    __url = 'https://tile.openstreetmap.org/{z}/{x}/{y}.png'

    SLOT_nameChanged(oldValue, newValue){
        if(newValue === 'osm') this.__url = 'https://tile.openstreetmap.org/{z}/{x}/{y}.png'
    }

    SLOT_parametersChanged(oldValue, newValue){
        for(let parameter of newValue){
            if(parameter.name === 'osm.mapping.host') this.__url = parameter.value
        }
    }
}



module.exports = Plugin