const Item = require("./Item")
const Bool = require("../QtQml/Bool")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")


class Canvas extends Item {
    static Immediate = 0
    static Threaded = 1
    static Cooperative = 2
    static Image = 0

    static meta = Object.assign({}, Item.meta, {
        available: {type: Bool, value: true},
        canvasSize: {type: Bool, value: true},
        antialiasing: {type: Bool, value: false},
        context: {type: Var, value: undefined},
        renderStrategy: {type: Real, value: Canvas.Immediate},
        renderTarget: {type: Real, value: Canvas.Image},
        contextType: {type: String, value: ''},

        availableChanged: {type:Signal, args:[]},
        canvasSizeChanged: {type:Signal, args:[]},
        antialiasingChanged: {type:Signal, args:[]},
        contextChanged: {type:Signal, args:[]},
        renderStrategyChanged: {type:Signal, args:[]},
        renderTargetChanged: {type:Signal, args:[]},
        contextTypeChanged: {type:Signal, args:[]},

        imageLoaded: {type:Signal, args:[]},
        painted: {type:Signal, args:[]},
        paint: {type:Signal, args:['region']},
    })

    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.__DOM.classList.add('Canvas')

        return obj
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

    SLOT_widthChanged(oldValue, newValue){
        super.SLOT_widthChanged(oldValue, newValue)
        this.__getDOM().setAttribute('width', newValue)
        if(newValue > 0 && this.height > 0) this.paint()
    }

    SLOT_heightChanged(oldValue, newValue){
        super.SLOT_heightChanged(oldValue, newValue)
        this.__getDOM().setAttribute('height', newValue)
        if(this.width > 0 && newValue > 0) this.paint()
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