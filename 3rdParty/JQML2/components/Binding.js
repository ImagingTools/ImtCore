const { QtObject } = require('./QtObject')
const { QVar, QString, QBool } = require('../utils/properties')

class Binding extends QtObject {
    static RestoreNone = 0
    static RestoreBinding = 1
    static RestoreValue = 2
    static RestoreBindingOrValue = 3

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
    }
    
    static defaultProperties = {
        delayed: { type: QBool, value: false, changed: '$update' },
        property: { type: QString, value: '', changed: '$update' },
        restoreMode: { type: QVar, value: Binding.RestoreBindingOrValue, changed: '$update' },
        target: { type: QVar, value: undefined, changed: '$update' },
        value: { type: QVar, value: undefined },
        when: { type: QBool, value: true, changed: '$update' },

    }
    
    $update(){
        if(this.getPropertyValue('target') && this.getPropertyValue('property')){
            if(this.$prop) this.getProperty('value').removeSubscriber(this.$prop)
            
            let path = this.property.split('.')
            let prop = this.target

            while(path.length){
                let propName = path.shift()
                prop = prop.getProperty(propName)
            }

            this.$prop = prop
            prop.subscribe(this.getProperty('value'))
            prop.setCompute(()=>{return this.getPropertyValue('value')})
            prop.update()
        }
    }
}

module.exports.Binding = Binding