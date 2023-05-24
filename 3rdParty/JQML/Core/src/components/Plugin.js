import {QtObject} from './QtObject'

export class Plugin extends QtObject  {
    constructor(args) {
        super(args)
        this.$cP('name', 'osm').connect(this.$nameChanged.bind(this))

        // this.$url = 'https://tile.openstreetmap.org/{z}/{x}/{y}.png'
        this.$url = 'https://maps.wikimedia.org/osm-intl/{z}/{x}/{y}@2x.png'
    }

    $nameChanged(){
        if(this.name === 'osm') this.$url = 'https://maps.wikimedia.org/osm-intl/{z}/{x}/{y}@2x.png'
    }
}

QML.Plugin = Plugin