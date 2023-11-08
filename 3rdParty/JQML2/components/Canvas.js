const { Item } = require('./Item')
const { QBool, QReal, QString, QVar } = require('../utils/properties')

class Canvas extends Item {
    static Immediate = 0
    static Threaded = 1
    static Cooperative = 2
    static Image = 0

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
        if(this.parent() && this.parent().$dom){
            this.$dom = document.createElement('canvas')
            this.parent().$dom.appendChild(this.$dom)
            this.setStyle(style)
            this.parent().addDomChild(this)
            this.$dom.id = this.UID
            this.$dom.classList.add(this.constructor.name)
        }
    }

    widthChanged(){
        super.$widthChanged()
        this.getDom().setAttribute('width', this.getPropertyValue('width'))
        if(this.$signals.paint) this.$signals.paint()
    }
    heightChanged(){
        super.$heightChanged()
        this.getDom().setAttribute('height', this.getPropertyValue('height'))
        if(this.$signals.paint) this.$signals.paint()
    }

    cancelRequestAnimationFrame(handle){

    }
    getContext(contextId, ...args){
        return this.getDom().getContext(contextId)
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

module.exports.Canvas = Canvas