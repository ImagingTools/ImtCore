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

    static create(parent=null, model=null, meta={}, properties=[], isRoot=true){
        let obj = super.create(parent, model, meta, properties, isRoot)
        this.parameters = []
        this.__url = 'https://tile.openstreetmap.org/{z}/{x}/{y}.png'

        return obj
    }

    onNameChanged(){
        if(this.name === 'osm') this.__url = 'https://tile.openstreetmap.org/{z}/{x}/{y}.png'
    }

    onParametersChanged(){
        for(let parameter of this.parameters){
            if(parameter.name === 'osm.mapping.host') this.__url = parameter.value
        }
    }
}

Plugin.initialize()

module.exports = Plugin