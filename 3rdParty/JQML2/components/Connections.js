const { QtObject } = require('./QtObject')
const { QVar, QBool } = require('../utils/properties')

class Connections extends QtObject {
    static defaultProperties = {
        target: { type: QVar, value: undefined, changed: '$targetChanged' },
        enabled: { type: QBool, value: true },
        ignoreUnknownSignals: { type: QBool, value: false },

    }
    $getConnections(){
        let res = []
        for(let key in this){
            if(typeof this[key] === 'function' && key.slice(0, 2) === 'on'){
                res.push({
                    signalName: key[2].toLowerCase() + key.slice(3),
                    slotName: key,
                })
            }
        }
        return res
    }
    $targetChanged(){
        let connections = this.$getConnections()
        if(this.$target){
            for(let connection of connections){
                let signal = this.$target.getSignal(connection.signalName)
                if(!signal) signal = this.$target.getSignal(connection.signalName.replaceAll('Changed', ''))
                if(!signal) {
                    let property = this.$target.getProperty(connection.signalName.replaceAll('Changed', ''))
                    if(property) signal = property.getNotify()
                }
                if(signal) signal.disconnect(this, this[connection.slotName])
            }
        }
        this.$target = this.getPropertyValue('target')
        if(!this.$target) return
        for(let connection of connections){
            let signal = this.$target.getSignal(connection.signalName)
            if(!signal) signal = this.$target.getSignal(connection.signalName.replaceAll('Changed', ''))
            if(!signal) {
                let property = this.$target.getProperty(connection.signalName.replaceAll('Changed', ''))
                if(property) signal = property.getNotify()
            }
            if(signal) signal.connect(this, this[connection.slotName])
        }
    }
}

module.exports.Connections = Connections