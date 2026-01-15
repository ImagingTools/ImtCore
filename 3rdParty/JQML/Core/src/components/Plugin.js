import {QtObject} from './QtObject'

export class Plugin extends QtObject  {
    constructor(args) {
        super(args)
        this.$cP('name', 'osm', this.$nameChanged)
        this.$cP('parameters', [], this.$parametersChanged)

        // this.$url = 'https://tile.openstreetmap.org/{z}/{x}/{y}.png'
        this.$url = 'https://maps.wikimedia.org/osm-intl/{z}/{x}/{y}@2x.png'
    }

    $parametersChanged(){
        for(let parameter of this.parameters){
            if(parameter.name === 'osm.mapping.host') this.$url = parameter.value
        }
    }

    $nameChanged(){
        if(this.name === 'osm') this.$url = 'https://maps.wikimedia.org/osm-intl/{z}/{x}/{y}@2x.png'
    }
}

QML.Plugin = Plugin