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

            if(oldValue){
                let sig = oldValue[signalName]
                if(sig && sig.disconnect) sig.disconnect(this, this[slotName])
                else if(!this.ignoreUnknownSignals) console.warn(`Connections: signal '${signalName}' not found on target (disconnect)`)
            }
            if(newValue){
                let sig = newValue[signalName]
                if(sig && sig.connect) sig.connect(this, this[slotName])
                else if(!this.ignoreUnknownSignals) console.warn(`Connections: signal '${signalName}' not found on target (connect)`)
            }
        }
    }

    __destroy(){
        for(let signalName in this.__connectionsInfo){
            let slotName = this.__connectionsInfo[signalName]
        
            if(this.target){
                let sig = this.target[signalName]
                if(sig && sig.disconnect) sig.disconnect(this, this[slotName])
            }
        }

        super.__destroy()
    }
}



module.exports = Connections