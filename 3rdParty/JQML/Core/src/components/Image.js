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

    $wAuto = true
    $hAuto = true
    $swAuto = true
    $shAuto = true

    constructor(parent) {
        super(parent)

        this.$cP('progress', 0).connect(this.$progressChanged.bind(this))
        this.$cP('fillMode', Image.Stretch).connect(this.$fillModeChanged.bind(this))
        this.$cP('source', '').connect(this.$sourceChanged.bind(this))

        this.$cPC('sourceSize', {
            width: 0,
            height: 0,
        }).connect(this.$sourceSizeChanged.bind(this))

        this.$img = new window.Image();
        this.$imgLoad = function(){
            if(this.$swAuto){
                // this.sourceSize.width = this.$img.naturalWidth
                if(this.$p['sourceSize.width'].val !== this.$p['sourceSize.width'].val){
                    this.$p['sourceSize.width'].val = this.$p['sourceSize.width'].val
                    this.$sourceChanged()
                }
                
                this.$swAuto = true
            }
            if(this.$shAuto){
                // this.sourceSize.height = this.$img.naturalHeight;
                if(this.$p['sourceSize.height'].val !== this.$p['sourceSize.height'].val){
                    this.$p['sourceSize.height'].val = this.$p['sourceSize.height'].val
                    this.$sourceChanged()
                }
                
                this.$shAuto = true
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
    $widthChanged(){
        super.$widthChanged()
        this.$wAuto = false
    }
    $heightChanged(){
        super.$heightChanged()
        this.$hAuto = false
    }
    $sourceSizeChanged(){
        // this.width = this.sourceSize.width
        // this.height = this.sourceSize.height
        if(this.$wAuto){
            // this.width = this.sourceSize.width
            if(this.$p.width.val !== this.$p['sourceSize.width'].val){
                this.$p.width.val = this.$p['sourceSize.width'].val
                this.$widthChanged()
            }
            
            this.$wAuto = true
            // this.$p.width.val = this.sourceSize.width
            // this.dom.style.width = `${this.width}px`
        }
        if(this.$hAuto){
            // this.height = this.sourceSize.height
            if(this.$p.height.val !== this.$p['sourceSize.height'].val){
                this.$p.height.val = this.$p['sourceSize.height'].val
                this.$heightChanged()
            }
            
            this.$hAuto = true
            // this.$p.height.val = this.sourceSize.height
            // this.dom.style.height = `${this.height}px`
        }
        this.$swAuto = false
        this.$shAuto = false
        this.$fillModeChanged()
    }
    $progressChanged(){

    }
    $fillModeChanged(){
        switch (this.fillMode) {
            case Image.Stretch:
                this.dom.style.backgroundRepeat = "no-repeat";
                this.dom.style.backgroundSize = `${this.$p['sourceSize.width'].val}px ${this.$p['sourceSize.height'].val}px`;
                this.dom.style.backgroundPosition = "center";
                break;
            case Image.Tile:
                this.dom.style.backgroundRepeat = "repeat";
                this.dom.style.backgroundSize = `${this.$p['sourceSize.width'].val}px ${this.$p['sourceSize.height'].val}px`;
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
                this.dom.style.backgroundSize = `${this.$p['sourceSize.width'].val}px ${this.$p['sourceSize.height'].val}px`;
                this.dom.style.backgroundPosition = "auto";
                break;
            case Image.TileHorizontally:
                this.dom.style.backgroundRepeat = "repeat-x";
                this.dom.style.backgroundSize = `${this.$p['sourceSize.width'].val}px ${this.$p['sourceSize.height'].val}px`;
                this.dom.style.backgroundPosition = "auto";
                break;
        }
    }
    $sourceChanged(){
        if(this.$p.source.val){
            this.progress = 0
            this.status = Image.Loading
            let source = this.$p.source.val.replaceAll(/[\n,\t, ]/g, '')
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