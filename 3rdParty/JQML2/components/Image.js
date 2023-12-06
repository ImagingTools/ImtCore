const { Item } = require('./Item')
const { QSourceSize, QString, QReal } = require('../utils/properties')

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

    static defaultProperties = {
        progress: { type: QReal, value: 0, changed: '$progressChanged' },
        source: { type: QString, value: '', changed: '$sourceChanged' },
        sourceSize: { type: QSourceSize, changed: '$sourceSizeChanged' },
        fillMode: { type: QReal, value: Image.Stretch, changed: '$fillModeChanged' },
        status: { type: QReal, value: Image.Null },
    }

    /*
    this.$cP('progress', 0, this.$progressChanged)
        this.$img = new window.Image();
        this.$imgLoad = function(){
            if(this.$sizeWidthAuto){
                this.sourceSize.width = this.$img.naturalWidth
                this.$sizeWidthAuto = true
            }
            if(this.$sizeHeightAuto){
                this.sourceSize.height = this.$img.naturalHeight
                this.$sizeHeightAuto = true
            }

            this.dom.style.backgroundImage = `url("${this.$img.src}")`

            this.progress = 1;
            this.status = Image.Ready;
        }.bind(this)
        this.$imgError = function(){
            this.status = Image.Error;
        }.bind(this)
        this.$img.addEventListener("load", this.$imgLoad);
        this.$img.addEventListener("error", this.$imgError);
    */

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        
        this.setStyle({
            backgroundRepeat: "no-repeat",
            backgroundPosition: "center",
        })
    }

    $sourceChanged(){
        if(!this.source) {
            this.setStyle({
                backgroundImage: 'none'
            })
            return
        }

        this.$img = new WebImage();
        let path = rootPath+'/'+this.getProperty('source').get().replaceAll('../','')
        this.$img.onload = ()=>{
            if(!this.$img) return
            this.getProperty('sourceSize').getProperty('width').setAuto(this.$img.naturalWidth)
            this.getProperty('sourceSize').getProperty('height').setAuto(this.$img.naturalHeight)

            // this.dom.style.backgroundImage = `url("${this.$this.$img.src}")`
            
            this.setStyle({
                backgroundImage: `url("${path.replaceAll('//','/')}")`
            })

            this.getProperty('progress').reset(1)
            this.getProperty('status').reset(Image.Ready)
            this.$img.remove()
            delete this.$img
        }
        this.$img.onerror = ()=>{
            this.getProperty('status').reset(Image.Error)
            this.$img.remove()
            delete this.$img
        }

        this.getProperty('status').reset(Image.Loading)
        this.$img.src = path.replaceAll('//','/')
    }

    updateImage(){
        switch (this.getProperty('fillMode').get()) {
            case Image.Stretch: {
                this.setStyle({
                    backgroundRepeat: "no-repeat",
                    backgroundSize: `${this.getProperty('sourceSize').getProperty('width').get()}px ${this.getProperty('sourceSize').getProperty('height').get()}px`,
                    backgroundPosition: "center",
                })
                break
            }
            case Image.Tile: {
                this.setStyle({
                    backgroundRepeat: "repeat",
                    backgroundSize: `${this.getProperty('sourceSize').getProperty('width').get()}px ${this.getProperty('sourceSize').getProperty('height').get()}px`,
                    backgroundPosition: "top left",
                })
                break
            }
            case Image.PreserveAspectFit: {
                this.setStyle({
                    backgroundRepeat: "no-repeat",
                    backgroundSize: `contain`,
                    backgroundPosition: "center",
                })
                break
            }
            case Image.PreserveAspectCrop: {
                this.setStyle({
                    backgroundRepeat: "no-repeat",
                    backgroundSize: `cover`,
                    backgroundPosition: "center",
                })
                break
            }
            case Image.TileVertically: {
                this.setStyle({
                    backgroundRepeat: "repeat-y",
                    backgroundSize: `${this.getProperty('sourceSize').getProperty('width').get()}px ${this.getProperty('sourceSize').getProperty('height').get()}px`,
                    backgroundPosition: "auto",
                })
                break
            }
            case Image.TileHorizontally: {
                this.setStyle({
                    backgroundRepeat: "repeat-x",
                    backgroundSize: `${this.getProperty('sourceSize').getProperty('width').get()}px ${this.getProperty('sourceSize').getProperty('height').get()}px`,
                    backgroundPosition: "auto",
                })
                break
            }
        }
    }

    $fillModeChanged(){
        this.updateImage()
    }

    $sourceSizeChanged(){
        this.updateImage()
    }

    $destroy(){
        if(this.$img) this.$img.remove()
        super.$destroy()
    }
}

module.exports.Image = Image