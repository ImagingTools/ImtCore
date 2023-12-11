const { QtObject } = require('./QtObject')
const { QBorder, QReal, QBool, QVar, QColor } = require('../utils/properties')

class MapPolygon extends QtObject {
    static defaultProperties = {
        autoFadeIn: { type: QBool, value: true },
        backend: { type: QVar, value: undefined },
        path: { type: QVar, value: undefined, changed: '$updateFeature' },
        color: { type: QColor, value: 'transparent', changed: '$updateFeature' },
        opacity: { type: QReal, value: 1, changed: '$updateFeature' },
        border: { type: QBorder, changed: '$updateFeature' },

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
            geometry: new OpenLayers.Polygon(
                [[
                    ...path
                ]]
            ),
        })
        this.$updateStyleFeature()
        if(added) this.parent.addMapItem(this)
    }
    $updateStyleFeature(){
        if(this.$feature){
            this.$feature.setStyle(new OpenLayers.Style({
                fill: new OpenLayers.Fill({
                    color: this.color,
                }),
                stroke: new OpenLayers.Stroke({
                    color: this.border.color,
                    width: this.border.width,
                })
            }))
        }
    }

    $destroy(){
        if(this.$feature && this.parent.$map) this.parent.$source.removeFeature(this.$feature)
        super.$destroy()
    }
   
}

module.exports.MapPolygon = MapPolygon