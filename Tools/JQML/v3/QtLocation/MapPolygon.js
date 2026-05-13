const QtObject = require("../QtQml/QtObject")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const Color = require("../QtQml/Color")
const Border = require("../QtQml/Border")
const GeoCoordinate = require("../QtPositioning/GeoCoordinate")
const Signal = require("../QtQml/Signal")


class MapPolygon extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        autoFadeIn: { type: Bool, value: true },
        backend: { type: Var },
        path: { type: Var },
        color: { type: Color, value: 'transparent' },
        opacity: { type: Real, value: 1 },
        border: { type: Border },
   
        autoFadeInChanged: { type: Signal },
        backendChanged: { type: Signal },
        pathChanged: { type: Signal },
        colorChanged: { type: Signal },
        opacityChanged: { type: Signal },
    })

    path = []

    SLOT_pathChanged(oldValue, newValue){
        this.__updateFeature()
    }


    SLOT_colorChanged(oldValue, newValue){
        this.__updateFeature()
    }

    SLOT_opacityChanged(oldValue, newValue){
        this.__updateFeature()
    }

    SLOT_borderChanged(oldValue, newValue){
        this.__updateFeature()
    }

    __setDOMStyle(){
        // this.__updateFeature()
    }

    __updateFeature(added = false){
        if(!this.path && !this.path.length) return
        if(!this.parent.__source){
            this.parent.__queue.add(this)
            return
        }
        if(this.__feature){
            this.parent.__source.removeFeature(this.__feature)
        }
        let path = []
        for(let p of this.path){
            path.push(OpenLayers.transform([p.longitude, p.latitude], 'EPSG:4326','EPSG:3857'))
        }
        this.__feature = new OpenLayers.Feature({
            geometry: new OpenLayers.Polygon(
                [[
                    ...path
                ]]
            ),
        })
        this.__updateStyleFeature()
        if(added) this.parent.addMapItem(this)
    }
    __updateStyleFeature(){
        if(this.__feature){
            this.__feature.setStyle(new OpenLayers.Style({
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
    __destroy(){
        if(this.__feature && this.parent.__map) this.parent.__source.removeFeature(this.__feature)
 
        super.destroy()
    }
}



module.exports = MapPolygon