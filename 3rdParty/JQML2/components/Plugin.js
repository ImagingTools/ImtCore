const { QtObject } = require('./QtObject')
const { QVar, QString } = require('../utils/properties')

class Plugin extends QtObject {
    static defaultProperties = {
        name: { type: QString, value: 'osm', changed: '$nameChanged' },
        parameters: { type: QVar, value: [], changed: '$parametersChanged' },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        // this.$url = 'https://maps.wikimedia.org/osm-intl/{z}/{x}/{y}@2x.png'
        this.$url = 'https://tile.openstreetmap.org/{z}/{x}/{y}.png'
    }

    $nameChanged(){
        if(this.getPropertyValue('name') === 'osm') this.$url = 'https://tile.openstreetmap.org/{z}/{x}/{y}.png'
    }

    $parametersChanged(){
        for(let parameter of this.getPropertyValue('parameters')){
            if(parameter.getPropertyValue('name') === 'osm.mapping.host') this.$url = parameter.getPropertyValue('value')
        }
    }
}

module.exports.Plugin = Plugin