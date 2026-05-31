import {Item} from './Item'

export class Canvas extends Item {

    static Immediate = 0
    static Threaded = 1
    static Cooperative = 2
    static Image = 0

    constructor(args) {
        super(args)

        this.$cP('available', true)
        this.$cP('canvasSize', true)
        this.$cP('context', null)
        this.$cP('contextType', '')
        this.$cP('renderStrategy', Canvas.Immediate)
        this.$cP('renderTarget', Canvas.Image)

        this.$cS('imageLoaded')
        this.$cS('paint', 'region')
        this.$cS('painted')
    }

    $domCreate(){
        super.$domCreate("canvas")

    }

    $widthChanged(){
        super.$widthChanged()
        this.dom.setAttribute('width', this.width)
        this.paint()
    }
    $heightChanged(){
        super.$heightChanged()
        this.dom.setAttribute('height', this.height)
        this.paint()
    }

    cancelRequestAnimationFrame(handle){

    }
    getContext(contextId, ... args){
        return this.dom.getContext(contextId)
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

QML.Canvas = Canvas