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

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        
        this.setStyle({
            backgroundRepeat: "no-repeat",
            backgroundPosition: "center",
        })
    }

    $sourceChanged(){
        if(!this.getPropertyValue('source')) {
            this.setStyle({
                backgroundImage: 'none'
            })
            return
        }

        let url = (rootPath+'/'+this.getProperty('source').get().replaceAll('../','')).replaceAll('qrc:','').replaceAll('//','/')
        this.$url = url

        this.getProperty('status').reset(Image.Loading)

        ImageController.load(url, (img)=>{
            if(!this.UID || this.$url !== url) return
            this.getProperty('sourceSize').getProperty('width').setAuto(img.width)
            this.getProperty('sourceSize').getProperty('height').setAuto(img.height)

            this.setStyle({
                backgroundImage: `url("${img.data}")`
            })

            this.getProperty('progress').reset(1)
            this.getProperty('status').reset(Image.Ready)
        },()=>{
            if(!this.UID) return
            this.getProperty('status').reset(Image.Error)
        })
        
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
}

module.exports.Image = Image