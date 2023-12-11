const { QtObject } = require('./QtObject')
const { QLine, QBool, QVar } = require('../utils/properties')

class MapPolyline extends QtObject {
    static defaultProperties = {
        autoFadeIn: { type: QBool, value: true },
        backend: { type: QVar, value: undefined },
        path: { type: QVar, value: undefined, changed: '$updateFeature' },
        line: { type: QLine, changed: '$updateFeature' },

    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.getProperty('path').reset([])
    }

    $updateFeature(added = false){
        if(!this.path && !this.path.length) return
        if(!this.parent.$source){
            this.parent.$queue.add(this)
            return
        }
        if(this.$feature){
            this.parent.$source.removeFeature(this.$feature)
        }
        let path = []
        for(let p of this.path){
            path.push(OpenLayers.transform([p.longitude, p.latitude], 'EPSG:4326','EPSG:3857'))
        }
        this.$feature = new OpenLayers.Feature({
            geometry: new OpenLayers.LineString(path),
        })
        this.$updateStyleFeature()
        if(added) this.parent.addMapItem(this)
    }
    $updateStyleFeature(){
        if(this.$feature){
            this.$feature.setStyle(new OpenLayers.Style({
                stroke: new OpenLayers.Stroke({
                    color: this.line.color,
                    width: this.line.width,
                })
            }))
        }
    }
    $destroy(){
        if(this.$feature && this.parent.$map) this.parent.$source.removeFeature(this.$feature)
        super.$destroy()
    }
   
}

module.exports.MapPolyline = MapPolyline