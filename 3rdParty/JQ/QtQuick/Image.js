const Item = require("./Item")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Var = require("../QtQml/Var")
const SourceSize = require("../QtQml/SourceSize")
const Signal = require("../QtQml/Signal")

class Image extends Item {
    static Stretch = 0
    static PreserveAspectFit = 1
    static PreserveAspectCrop = 2
    static Tile = 3
    static TileVertically = 4
    static TileHorizontally = 5
    static Pad = 6
    static Stretch = 7

    static Null = 0
    static Ready = 1
    static Loading = 2
    static Error = 3

    static AlignLeft = 0
    static AlignRight = 1
    static AlignHCenter = 2

    static AlignTop = 0
    static AlignBottom = 1
    static AlignVCenter = 2

    static meta = Object.assign({}, Item.meta, {
        progress: {type: Real, value:0, signalName:'progressChanged'},
        source: {type: String, value:'', signalName:'sourceChanged'},
        sourceSize: {type: SourceSize},
        fillMode: {type: Real, value:Image.Stretch, signalName:'fillModeChanged'},
        status: {type: Real, value:Image.Null, signalName:'statusChanged'},
        paintedWidth: {type:Real, value:0, signalName:'paintedWidthChanged'},
        paintedHeight: {type:Real, value:0, signalName:'paintedHeightChanged'},
        
        progressChanged: {type:Signal, slotName:'onProgressChanged', args:[]},
        sourceChanged: {type:Signal, slotName:'onSourceChanged', args:[]},
        fillModeChanged: {type:Signal, slotName:'onFillModeChanged', args:[]},
        statusChanged: {type:Signal, slotName:'onStatusChanged', args:[]},
        paintedWidthChanged: {type:Signal, slotName:'onPaintedWidthChanged', args:[]},
        paintedHeightChanged: {type:Signal, slotName:'onPaintedHeightChanged', args:[]},
    })

    static create(parent, model, ...args){
        let proxy = super.create(parent, model, ...args)
        proxy.__DOM.classList.add('Image')

        return proxy
    }

    onSourceChanged(){
        if(!this.source) {
            this.__setDOMStyle({
                backgroundImage: 'none'
            })
            return
        }

        let url = '/ControlsGallery/Views/' + this.source
        this.$url = url

        this.status = Image.Loading

        JQApplication.ImageController.load(url, (img)=>{
            if(this.$url !== url) return
            this.sourceSize.width = img.width
            this.sourceSize.height = img.height

            this.__setDOMStyle({
                backgroundImage: `url("${img.data}")`
            })

            this.progress = 1
            this.status = Image.Ready
        },()=>{
            if(!this.UID) return
            this.status = Image.Error
        })
        
    }

    __updateImage(){
        switch (this.fillMode) {
            case Image.Stretch: {
                this.__setDOMStyle({
                    backgroundRepeat: "no-repeat",
                    backgroundSize: `${this.sourceSize.width}px ${this.sourceSize.height}px`,
                    backgroundPosition: "center",
                })
                break
            }
            case Image.Tile: {
                this.__setDOMStyle({
                    backgroundRepeat: "repeat",
                    backgroundSize: `${this.sourceSize.width}px ${this.sourceSize.height}px`,
                    backgroundPosition: "top left",
                })
                break
            }
            case Image.PreserveAspectFit: {
                this.__setDOMStyle({
                    backgroundRepeat: "no-repeat",
                    backgroundSize: `contain`,
                    backgroundPosition: "center",
                })
                break
            }
            case Image.PreserveAspectCrop: {
                this.__setDOMStyle({
                    backgroundRepeat: "no-repeat",
                    backgroundSize: `cover`,
                    backgroundPosition: "center",
                })
                break
            }
            case Image.TileVertically: {
                this.__setDOMStyle({
                    backgroundRepeat: "repeat-y",
                    backgroundSize: `${this.sourceSize.width}px ${this.sourceSize.height}px`,
                    backgroundPosition: "auto",
                })
                break
            }
            case Image.TileHorizontally: {
                this.__setDOMStyle({
                    backgroundRepeat: "repeat-x",
                    backgroundSize: `${this.sourceSize.width}px ${this.sourceSize.height}px`,
                    backgroundPosition: "auto",
                })
                break
            }
        }
    }

    onFillModeChanged(){
        this.__updateImage()
    }
}

module.exports = Image