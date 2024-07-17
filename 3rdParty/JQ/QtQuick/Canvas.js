const Item = require("./Item")
const Bool = require("../QtQml/Bool")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")
const Var = require("../QtQml/Var")

class Canvas extends Item {
    static Immediate = 0
    static Threaded = 1
    static Cooperative = 2
    static Image = 0

    static meta = Object.assign({}, Item.meta, {
        available: {type: Bool, value: true, signalName: 'availableChanged' },
        canvasSize: {type: Bool, value: true, signalName: 'canvasSizeChanged' },
        context: {type: Var, value: undefined, signalName: 'contextChanged' },
        renderStrategy: {type: Real, value: Canvas.Immediate, signalName: 'renderStrategyChanged' },
        renderTarget: {type: Real, value: Canvas.Image, signalName: 'renderTargetChanged' },
        contextType: {type: String, value: '', signalName: 'contextTypeChanged' },

        availableChanged: {type:Signal, slotName:'onAvailableChanged', args:[]},
        canvasSizeChanged: {type:Signal, slotName:'onCanvasSizeChanged', args:[]},
        contextChanged: {type:Signal, slotName:'onContextChanged', args:[]},
        renderStrategyChanged: {type:Signal, slotName:'onRenderStrategyChanged', args:[]},
        renderTargetChanged: {type:Signal, slotName:'onRenderTargetChanged', args:[]},
        contextTypeChanged: {type:Signal, slotName:'onContextTypeChanged', args:[]},

        imageLoaded: {type:Signal, slotName:'onImageLoaded', args:[]},
        painted: {type:Signal, slotName:'onPainted', args:[]},
        paint: {type:Signal, slotName:'onPaint', args:['region']},
    })

    cancelRequestAnimationFrame(handle){

    }
    getContext(contextId, ...args){
        
    }
    isImageError(image){

    }
    isImageLoaded(image){

    }
    isImageLoading(image){

    }
    loadImage(image){

    }
    markDirty(area){

    }
    requestAnimationFrame(callback){

    }
    requestPaint(){
        
    }
    save(filename, imageSize){

    }
    toDataURL(mimeType){

    }
    unloadImage(image){

    }
}

module.exports = Canvas