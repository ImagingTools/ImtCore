const QtObject = require("./QtObject")
const Bool = require("./Bool")
const Var = require("./Var")
const Signal = require("./Signal")

class Connections extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        enabled: { type: Bool, value: true, signalName: 'enabledChanged' },
        target: { type: Var, value: undefined, signalName: 'targetChanged' },
        ignoreUnknownSignals: { type: Bool, value: false, signalName: 'ignoreUnknownSignalsChanged' },

        enabledChanged: {type:Signal, slotName:'onEnabledChanged', args:[]},
        targetChanged: {type:Signal, slotName:'onTargetChanged', args:[]},
        ignoreUnknownSignalsChanged: {type:Signal, slotName:'onIgnoreUnknownSignalsChanged', args:[]},
    })

    __target = null
    __connectionsInfo = {}

    onTargetChanged(){
        for(let signalName in this.__connectionsInfo){
            let slotName = this.__connectionsInfo[signalName]
        
            if(this.__target){
                if(this.__target[signalName] instanceof Signal) this.__target[signalName].disconnect(this, this[slotName])
            }
        }

        this.__target = this.target

        for(let signalName in this.__connectionsInfo){
            let slotName = this.__connectionsInfo[signalName]
        
            if(this.__target){
                if(this.__target[signalName] instanceof Signal) this.__target[signalName].connect(this, this[slotName])
            }
        }
    }

    __destroy(){
        for(let signalName in this.__connectionsInfo){
            let slotName = this.__connectionsInfo[signalName]
        
            if(this.__target){
                if(this.__target[signalName] instanceof Signal) this.__target[signalName].disconnect(this, this[slotName])
            }
        }

        delete this.__target

        super.__destroy()
    }
}

module.exports = Connections