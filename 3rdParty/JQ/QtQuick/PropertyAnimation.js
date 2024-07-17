const Animation = require("./Animation")
const String = require("../QtQml/String")
const Real = require("../QtQml/Real")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")

class PropertyAnimation extends Animation {
    static meta = Object.assign({}, Animation.meta, {
        duration: { type: Real, value: 250, signalName: 'durationChanged' },
        to: { type: Var, value: undefined, signalName: 'toChanged' },
        from: { type: Var, value: undefined, signalName: 'fromChanged' },
        property: { type: String, value: '', signalName: 'propertyChanged'},
        properties: { type: String, value: '', signalName: 'propertiesChanged' },
        target: { type: Var, value: undefined, signalName: 'targetChanged' },

        durationChanged: { type:Signal, slotName:'onDurationChanged', args:[] },
        toChanged: { type:Signal, slotName:'onToChanged', args:[] },
        fromChanged: { type:Signal, slotName:'onFromChanged', args:[] },
        propertyChanged: { type:Signal, slotName:'onPropertyChanged', args:[] },
        propertiesChanged: { type:Signal, slotName:'onPropertiesChanged', args:[] },
        targetChanged: { type:Signal, slotName:'onTargetChanged', args:[] },
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        let self = proxy.__self 

        return proxy
    }

    restart() {

    }
 
}

module.exports = PropertyAnimation