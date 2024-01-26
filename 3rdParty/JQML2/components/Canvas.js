const { Item } = require('./Item')
const { QBool, QReal, QString, QVar } = require('../utils/properties')

class Canvas extends Item {
    static Immediate = 0
    static Threaded = 1
    static Cooperative = 2
    static Image = 0

    $cache = {}

    static defaultProperties = {
        available: { type: QBool, value: true },
        canvasSize: { type: QBool, value: true },
        context: { type: QVar, value: undefined },
        renderStrategy: { type: QReal, value: Canvas.Immediate },
        renderTarget: { type: QReal, value: Canvas.Image },
        contextType: { type: QString, value: '' },

    }

    static defaultSignals = {
        imageLoaded: { params: [] },
		painted: { params: [] },
		paint: { params: ['region'] },
    }

    createDom(tag = 'canvas', style){
        if(this.parent && this.parent.$dom){
            this.$dom = document.createElement('canvas')
            this.parent.$dom.appendChild(this.$dom)
            this.setStyle(style)
            this.parent.addDomChild(this)
            this.$dom.id = this.UID
            this.$dom.classList.add(this.constructor.name)
        }
    }

    $widthChanged(){
        super.$widthChanged()
        this.getDom().setAttribute('width', this.getPropertyValue('width'))
        if(this.$signals.paint) this.$signals.paint()
    }
    $heightChanged(){
        super.$heightChanged()
        this.getDom().setAttribute('height', this.getPropertyValue('height'))
        if(this.$signals.paint) this.$signals.paint()
    }

    cancelRequestAnimationFrame(handle){

    }
    getContext(contextId, ...args){
        let ctx = this.getDom().getContext(contextId)
        ctx.roundedRect = (...args)=>{ctx.roundRect(...args)}
        let originDrawImage = ctx.drawImage
        ctx.drawImage = (...args)=>{
            
            if(typeof args[0] === 'string'){
                let path = rootPath+'/'+args[0].replaceAll('../','')
                if(this.$cache[path]){
                    args[0] = this.$cache[path]
                    originDrawImage.call(ctx, ...args)
                } else {
                    let img = new OriginImage();
                    img.onload = ()=>{
                        args[0] = img
                        originDrawImage.call(ctx, ...args)
                        this.$cache[path] = img
                        // img.remove()
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
        if(this.$signals.paint) this.$signals.paint()
    }
    save(filename, imageSize){

    }
    toDataURL(mimeType){

    }
    unloadImage(image){

    }

    $destroy(){
        for(let key in this.$cache){
            this.$cache[key].remove()
        }
        super.$destroy()
    }
}

module.exports.Canvas = Canvas