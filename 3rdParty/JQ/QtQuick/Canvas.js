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
        antialiasing: {type: Bool, value: false, signalName: 'antialiasingChanged' },
        context: {type: Var, value: undefined, signalName: 'contextChanged' },
        renderStrategy: {type: Real, value: Canvas.Immediate, signalName: 'renderStrategyChanged' },
        renderTarget: {type: Real, value: Canvas.Image, signalName: 'renderTargetChanged' },
        contextType: {type: String, value: '', signalName: 'contextTypeChanged' },

        availableChanged: {type:Signal, slotName:'onAvailableChanged', args:[]},
        canvasSizeChanged: {type:Signal, slotName:'onCanvasSizeChanged', args:[]},
        antialiasingChanged: {type:Signal, slotName:'onAntialiasingChanged', args:[]},
        contextChanged: {type:Signal, slotName:'onContextChanged', args:[]},
        renderStrategyChanged: {type:Signal, slotName:'onRenderStrategyChanged', args:[]},
        renderTargetChanged: {type:Signal, slotName:'onRenderTargetChanged', args:[]},
        contextTypeChanged: {type:Signal, slotName:'onContextTypeChanged', args:[]},

        imageLoaded: {type:Signal, slotName:'onImageLoaded', args:[]},
        painted: {type:Signal, slotName:'onPainted', args:[]},
        paint: {type:Signal, slotName:'onPaint', args:['region']},
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        proxy.__DOM.classList.add('Canvas')

        return proxy
    }

    __cache = {}
    __init = false

    __getDOM(){
        if(this.__DOM) {
            return this.__DOM
        } else {
            let dom = document.createElement('canvas')
            this.__DOM = dom
            return dom
        }
    }

    __initCtx(ctx){
        if(this.__init) return

        ctx.roundedRect = (...args)=>{ctx.roundRect(...args)}
        let originDrawImage = ctx.drawImage
        ctx.drawImage = (...args)=>{
            if(typeof args[0] === 'string'){
                let path = JQApplication.rootPath+'/'+args[0].replaceAll('../','')
                if(this.__cache[path]){
                    args[0] = this.__cache[path]
                    originDrawImage.call(ctx, ...args)
                } else {
                    let img = new Image();
                    img.onload = ()=>{
                        args[0] = img
                        originDrawImage.call(ctx, ...args)
                        this.__cache[path] = img
                        this.requestPaint()
                    }
                    img.onerror = ()=>{
                        img.remove()
                    }

                    img.src = path.replaceAll('//','/')
                }
                
            } else {
                originDrawImage.call(ctx, ...args)
            }
            
        }

        this.__init = true
    }

    onWidthChanged(){
        super.onWidthChanged()
        this.__getDOM().setAttribute('width', this.width)
        if(this.width > 0 && this.height > 0) this.paint()
    }

    onHeightChanged(){
        super.onHeightChanged()
        this.__getDOM().setAttribute('height', this.height)
        if(this.width > 0 && this.height > 0) this.paint()
    }

    cancelRequestAnimationFrame(handle){

    }
    getContext(contextId, ...args){
        let ctx = this.__getDOM().getContext(contextId)
        this.__initCtx(ctx)
        return ctx
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
        if(this.width > 0 && this.height > 0) this.paint()
    }
    save(filename, imageSize){

    }
    toDataURL(mimeType){

    }
    unloadImage(image){

    }
}

module.exports = Canvas