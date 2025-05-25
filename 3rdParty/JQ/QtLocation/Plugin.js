const QtObject = require("../QtQml/QtObject")
const String = require("../QtQml/String")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")


class Plugin extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        name: {type: String, value:'osm', signalName:'nameChanged'},
        parameters: {type: Var, signalName:'parametersChanged'},

        nameChanged: {type:Signal, slotName:'onNameChanged', args:[]},
        parametersChanged: {type:Signal, slotName:'onParametersChanged', args:[]},
   
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