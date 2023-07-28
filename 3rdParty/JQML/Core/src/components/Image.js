import { Item } from './Item'
import {Signal} from '../utils/Signal'

export class Image extends Item {
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

    $widthAuto = true
    $heightAuto = true
    $sizeWidthAuto = true
    $sizeHeightAuto = true

    constructor(args) {
        super(args)

        this.$cP('progress', 0, this.$progressChanged)
        this.$cP('fillMode', Image.Stretch, this.$fillModeChanged)
        this.$cP('source', '', this.$sourceChanged)

        this.$cPC('sourceSize', {
            width: 0,
            height: 0,
        }, this.$sourceSizeChanged)

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

    }

    $domCreate(){
        super.$domCreate()
        this.dom.style.backgroundRepeat = "no-repeat";
        this.dom.style.backgroundPosition = "center";
    }
    $sourceSizeChanged(){
  
        this.$fillModeChanged()
    }
    $progressChanged(){

    }
    $fillModeChanged(){
        switch (this.fillMode) {
            case Image.Stretch:
                this.dom.style.backgroundRepeat = "no-repeat";
                this.dom.style.backgroundSize = `${this.sourceSize.width}px ${this.sourceSize.height}px`;
                this.dom.style.backgroundPosition = "center";
                break;
            case Image.Tile:
                this.dom.style.backgroundRepeat = "repeat";
                this.dom.style.backgroundSize = `${this.sourceSize.width}px ${this.sourceSize.height}px`;
                this.dom.style.backgroundPosition = "top left";
                break;
            case Image.PreserveAspectFit:
                this.dom.style.backgroundRepeat = "no-repeat";
                this.dom.style.backgroundSize = "contain";
                this.dom.style.backgroundPosition = "center";
                break;
            case Image.PreserveAspectCrop:
                this.dom.style.backgroundRepeat = "no-repeat";
                this.dom.style.backgroundSize = "cover";
                this.dom.style.backgroundPosition = "center";
                break;
            case Image.TileVertically:
                this.dom.style.backgroundRepeat = "repeat-y";
                this.dom.style.backgroundSize = `${this.sourceSize.width}px ${this.sourceSize.height}px`;
                this.dom.style.backgroundPosition = "auto";
                break;
            case Image.TileHorizontally:
                this.dom.style.backgroundRepeat = "repeat-x";
                this.dom.style.backgroundSize = `${this.sourceSize.width}px ${this.sourceSize.height}px`;
                this.dom.style.backgroundPosition = "auto";
                break;
        }
    }
    $sourceChanged(){
        if(this.source){
            this.progress = 0
            this.status = Image.Loading
            let source = this.source.replaceAll(/[\n,\t, ]/g, '')
            let path = []
            if(Core.rootPath !== ''){
                path.push(Core.rootPath)
                path.push(source.replaceAll('../', '').replaceAll('./', ''))
            } else {    
                // if(Core.hostPath !== '') path.push(Core.hostPath)
                // if(Core.rootPath !== '') path.push(Core.rootPath)
                if(this.$basePath !== '') path.push(this.$basePath)
                path.push(source)
            }
            
            let fullpath = path.join('/').replaceAll('//', '/')
            
            this.$img.src = fullpath
        } else {
            this.dom.style.backgroundImage = `none`
        }
    }

    $destroy(){
        // this.$img.outerHTML = this.$img.outerHTML
        this.$img.removeEventListener('load', this.$imgLoad)
        this.$img.removeEventListener('error', this.$imgError)
        this.$img.remove()
        super.$destroy()
    }
}

QML.Image = Image