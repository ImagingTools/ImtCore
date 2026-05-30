const { Animation } = require('./Animation')
const { QVar, QString } = require('../utils/properties')

class PropertyAction extends Animation {

    static defaultProperties = {
        target: { type: QVar, value: undefined },
        targets: { type: QVar, value: undefined },
        property: { type: QString, value: '' },
        properties: { type: QString, value: '' },
        value: { type: QVar, value: undefined },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.getProperty('target').reset(parent)
    }

    tick(){
        if(!this.getPropertyValue('running')) return

        const targets = []
        const t = this.getPropertyValue('target')
        if(t) targets.push(t)
        const ts = this.getPropertyValue('targets')
        if(Array.isArray(ts)) for(const x of ts) if(x) targets.push(x)

        const propsStr = this.getPropertyValue('properties') || this.getPropertyValue('property')
        const props = propsStr ? propsStr.split(',').map(s => s.trim()).filter(Boolean) : []
        const value = this.getPropertyValue('value')

        for(const tgt of targets){
            for(const p of props){
                if(tgt && tgt.getProperty && tgt.getProperty(p)){
                    tgt.getProperty(p).reset(value)
                }
            }
        }

        this.getProperty('running').reset(false)
        if(this.$signals.stopped) this.$signals.stopped()
        if(this.$signals.finished) this.$signals.finished()
    }
}

module.exports.PropertyAction = PropertyAction
