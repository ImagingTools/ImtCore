const { QtObject } = require('./QtObject')
const { QString } = require('../utils/properties')

class PluginParameter extends QtObject {
    static defaultProperties = {
        name: { type: QString, value: '', changed: '$parameterChanged' },
        value: { type: QString, value: '', changed: '$parameterChanged' },
    }

    constructor(parent){
        super(parent)
        
        parent.getPropertyValue('parameters').push(this)
    }

    $parameterChanged(){
        this.parent().getProperty('parameters').getNotify()()
    }

    $destroy(){
        let index = this.parent().getPropertyValue('parameters').indexOf(this)
        if(index >= 0) this.parent().getPropertyValue('parameters').splice(index, 1)
    }
}

module.exports.PluginParameter = PluginParameter