const QtObject = require("../QtQml/QtObject")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const Color = require("../QtQml/Color")
const Border = require("../QtQml/Border")
const GeoCoordinate = require("../QtPositioning/GeoCoordinate")
const Signal = require("../QtQml/Signal")


class MapPolyline extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        autoFadeIn: { type: Bool, value: true },
        backend: { type: Var },
        path: { type: Var },
        line: { type: Border },
   
        autoFadeInChanged: { type: Signal },
        backendChanged: { type: Signal },
        pathChanged: { type: Signal },
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
            geometry: new OpenLayers.LineString(path),
        })
        this.__updateStyleFeature()
        if(added) this.parent.addMapItem(this)
    }
    __updateStyleFeature(){
        if(this.__feature){
            this.__feature.setStyle(new OpenLayers.Style({
                stroke: new OpenLayers.Stroke({
                    color: this.line.color,
                    width: this.line.width,
                })
            }))
        }
    }
    __destroy(){
        if(this.__feature && this.parent.__map) this.parent.__source.removeFeature(this.__feature)
 
        super.destroy()
    }
}



module.exports = MapPolyline