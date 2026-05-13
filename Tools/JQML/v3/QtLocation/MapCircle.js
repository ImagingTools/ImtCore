const QtObject = require("../QtQml/QtObject")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const Color = require("../QtQml/Color")
const Border = require("../QtQml/Border")
const GeoCoordinate = require("../QtPositioning/GeoCoordinate")
const Signal = require("../QtQml/Signal")


class MapCircle extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        autoFadeIn: { type: Bool, value: true },
        backend: { type: Var },
        center: { type: GeoCoordinate },
        color: { type: Color, value: 'transparent' },
        opacity: { type: Real, value: 1 },
        radius: { type: Real, value: 0 },
        border: { type: Border },
   
        autoFadeInChanged: { type: Signal },
        backendChanged: { type: Signal },
        centerChanged: { type: Signal },
        colorChanged: { type: Signal },
        opacityChanged: { type: Signal },
        radiusChanged: { type: Signal },
    })

    SLOT_centerChanged(oldValue, newValue){
        this.__updateFeature()
    }

    SLOT_colorChanged(oldValue, newValue){
        this.__updateFeature()
    }

    SLOT_opacityChanged(oldValue, newValue){
        this.__updateFeature()
    }

    SLOT_radiusChanged(oldValue, newValue){
        if(this.__feature){
            this.__feature.getGeometry().setRadius(newValue)
        } else {
            this.__updateFeature()
        }
    }

    SLOT_borderChanged(oldValue, newValue){
        this.__updateFeature()
    }

    __setDOMStyle(){
        // this.__updateFeature()
    }

    __updateFeature(added = false){
        if(!this.center) return
        if(!this.parent.__source){
            this.parent.__queue.add(this)
            return
        }
        if(this.__feature){
            this.parent.__source.removeFeature(this.__feature)
        }
        this.__feature = new OpenLayers.Feature({
            geometry: new OpenLayers.Circle(
                OpenLayers.transform([this.center.longitude, this.center.latitude], 'EPSG:4326','EPSG:3857'),
                this.radius
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



module.exports = MapCircle