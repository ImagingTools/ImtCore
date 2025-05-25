const QtObject = require("../QtQml/QtObject")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const Color = require("../QtQml/Color")
const Border = require("../QtQml/Border")
const GeoCoordinate = require("../QtPositioning/GeoCoordinate")
const Signal = require("../QtQml/Signal")


class MapRectangle extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        autoFadeIn: { type: Bool, value: true },
        backend: { type: Var },
        topLeft: { type: GeoCoordinate },
        bottomRight: { type: GeoCoordinate }, 
        color: { type: Color, value: 'transparent' },
        opacity: { type: Real, value: 1 },
        border: { type: Border },
   
        autoFadeInChanged: { type: Signal },
        topLeftChanged: { type: Signal },
        bottomRightChanged: { type: Signal },
        backendChanged: { type: Signal },
        colorChanged: { type: Signal },
        opacityChanged: { type: Signal },
    })

    SLOT_topLeftChanged(oldValue, newValue){
        this.__updateFeature()
    }

    SLOT_bottomRightChanged(oldValue, newValue){
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
        if(!this.topLeft || !this.bottomRight) return
        if(!this.parent.__source){
            this.parent.__queue.add(this)
            return
        }
        if(this.__feature){
            this.parent.__source.removeFeature(this.__feature)
        }
        this.__feature = new OpenLayers.Feature({
            geometry: new OpenLayers.Polygon(
                [[
                    OpenLayers.transform([this.topLeft.longitude, this.topLeft.latitude], 'EPSG:4326','EPSG:3857'),
                    OpenLayers.transform([this.bottomRight.longitude, this.topLeft.latitude], 'EPSG:4326','EPSG:3857'),
                    OpenLayers.transform([this.bottomRight.longitude, this.bottomRight.latitude], 'EPSG:4326','EPSG:3857'),
                    OpenLayers.transform([this.topLeft.longitude, this.bottomRight.latitude], 'EPSG:4326','EPSG:3857')
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



module.exports = MapRectangle