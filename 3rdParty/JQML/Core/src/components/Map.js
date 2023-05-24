import {Item} from './Item'

import $Map from 'ol/Map.js';
import $OSM from 'ol/source/OSM.js';
import $TileLayer from 'ol/layer/Tile.js';
import $View from 'ol/View.js';
import $SourceVector from 'ol/source/Vector'
import $LayerVector from 'ol/layer/Vector'

export class Map extends Item {
    $queue = new Set()

    constructor(args) {
        Core.loadMapTools()
        super(args)

        this.$cP('plugin', undefined).connect(this.$pluginChanged.bind(this))
        this.$cP('center', QtPositioning.coordinate(0, 0)).connect(this.$centerChanged.bind(this))
        this.$cP('zoomLevel', 2).connect(this.$zoomLevelChanged.bind(this))

        this.$initialized = false
        this.$s['Component.completed'].connect(this.$init.bind(this))
    }

    $init(){
        if(this.$initialized) return

        this.$source = new $SourceVector({
			features: [],//new ol.format.GeoJSON().readFeatures(geojsonObject),
		});

		this.$layer = new $LayerVector({
			source: this.$source,
			//style: styles,
		});
		this.$map = new $Map({
			target: this.dom,
			layers: [
				new $TileLayer({
					source: new $OSM({
                        url: this.plugin.$url,
                        crossOrigin: 'anonymous',
                    }),
				}),
				this.$layer,
			],
			controls: [],
			view: new $View({
				center: this.center,
				zoom: this.zoomLevel,
			}),
		});
        this.$initialized = true

        for(let f of this.$queue){
            f.$updateFeature()
        }
        this.$queue.clear()
    }

    $domCreate(){
        super.$domCreate()
        //this.dom.style.backgroundColor = this.color
    }

    $pluginChanged(){

    }
    $centerChanged(){

    }
    $zoomLevelChanged(){

    }
    $destroy(){
        this.$queue.clear()
        super.$destroy()
    }
}

QML.Map = Map