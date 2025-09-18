const QtObject = require("./QtObject")
const Bool = require("./Bool")
const Var = require("./Var")
const Signal = require("./Signal")

class Connections extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        enabled: { type: Bool, value: true},
        target: { type: Var, value: undefined},
        ignoreUnknownSignals: { type: Bool, value: false},

        enabledChanged: {type:Signal, args:[]},
        targetChanged: {type:Signal, args:[]},
        ignoreUnknownSignalsChanged: {type:Signal, args:[]},
    })

    __connectionsInfo = {}

    SLOT_targetChanged(oldValue, newValue){
        for(let signalName in this.__connectionsInfo){
            let slotName = this.__connectionsInfo[signalName]

            if(oldValue) oldValue[signalName].disconnect(this, this[slotName])
            if(newValue) newValue[signalName].connect(this, this[slotName])
        }
    }

    __destroy(){
        for(let signalName in this.__connectionsInfo){
            let slotName = this.__connectionsInfo[signalName]
        
            if(this.target){
                this.target[signalName].disconnect(this, this[slotName])
            }
        }

        super.__destroy()
    }
}



module.exports = Connections