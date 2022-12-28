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
        super.$domCreate()

        this.dom = document.createElement("canvas")

        if(this.parent){
            this.parent.dom.appendChild(this.dom)
        }
        
        this.dom.style.position = 'absolute'
        this.dom.style.overflow = 'unset'
        this.dom.style.zIndex = this.z
        this.dom.classList.add(this.constructor.name)
        this.dom.id = `el-${this.UID}`

        this.dom.addEventListener("focusin", (e)=>{
			this.focus = true
		})
		this.dom.addEventListener("focusout", (e)=>{
			this.focus = false
		})

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