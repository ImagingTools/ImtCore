const engine = require('../Core/engine')
const { GroupObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { Item, bits } = require('./Item')

const STYLE = bits.STYLE

const Stretch = 0, PreserveAspectFit = 1, PreserveAspectCrop = 2, Tile = 3, TileVertically = 4, TileHorizontally = 5, Pad = 6
const Null = 0, Ready = 1, Loading = 2, Error = 3

// In the browser every image loads asynchronously; Qt loads local files synchronously unless `asynchronous`
// sourceSize: a size value type with width / height; unset parts are -1
class SourceSize extends GroupObject {}
defineType(SourceSize, {
    props: {
        width: { kind: 'real', initial: -1, effect: (size) => size.$owner.$updateSize() },
        height: { kind: 'real', initial: -1, effect: (size) => size.$owner.$updateSize() },
    },
})

class Image extends Item {
    constructor(parent = null){
        super(parent)
        this.$loader = null
        this.$natural = null      // { width, height } of the loaded image
    }

    $load(){
        let source = this.$v[Image.SOURCE]
        if(this.$loader){
            this.$loader.onload = this.$loader.onerror = null
            this.$loader = null
        }
        this.$natural = null
        if(!source){
            this.$write(Image.PROGRESS, 0)
            this.$write(Image.STATUS, Null)
            this.$updateSize()
            return
        }
        if(typeof document === 'undefined') return
        let loader = new window.Image()
        this.$loader = loader
        this.$write(Image.PROGRESS, 0)
        this.$write(Image.STATUS, Loading)
        loader.onload = () => engine.batch(() => {
            if(this.$loader !== loader) return
            this.$natural = { width: loader.naturalWidth, height: loader.naturalHeight }
            this.$updateSize()
            this.$write(Image.PROGRESS, 1)
            this.$write(Image.STATUS, Ready)
        })
        loader.onerror = () => engine.batch(() => {
            if(this.$loader !== loader) return
            engine.reportError('Cannot open: ' + source, 'Image')
            this.$write(Image.STATUS, Error)
        })
        let url = require('../Core/Resources').resolveUrl(source)
        this.$url = url
        loader.src = url
    }

    // implicit size: the loaded size scaled to sourceSize (one dimension keeps the aspect ratio)
    $updateSize(){
        let natural = this.$natural
        let width = 0, height = 0
        if(natural){
            width = natural.width
            height = natural.height
            let size = this.$v[Image.SOURCESIZE]
            if(size && (size.width > 0 || size.height > 0) && width > 0 && height > 0){
                let sx = size.width > 0 ? size.width / width : Infinity
                let sy = size.height > 0 ? size.height / height : Infinity
                let scale = Math.min(sx, sy, 1)
                width = Math.round(width * scale)
                height = Math.round(height * scale)
            }
        }
        this.$write(Item.IMPLICITWIDTH, width)
        this.$write(Item.IMPLICITHEIGHT, height)
        this.$updatePainted()
        engine.markDirty(this, STYLE)
    }

    $updatePainted(){
        let w = this.$v[Item.WIDTH], h = this.$v[Item.HEIGHT]
        let iw = this.$v[Item.IMPLICITWIDTH], ih = this.$v[Item.IMPLICITHEIGHT]
        let mode = this.$v[Image.FILLMODE]
        let pw = w, ph = h
        if(iw > 0 && ih > 0){
            if(mode === PreserveAspectFit){
                let scale = Math.min(w / iw, h / ih)
                pw = iw * scale
                ph = ih * scale
            } else if(mode === Pad){
                pw = iw
                ph = ih
            }
        }
        this.$write(Image.PAINTEDWIDTH, pw)
        this.$write(Image.PAINTEDHEIGHT, ph)
    }

    $paint(dirty){
        super.$paint(dirty)
        if(!(dirty & STYLE) || !this.$dom) return
        let s = this.$dom.style
        let source = this.$v[Image.SOURCE]
        let url = source ? this.$url : ''
        s.backgroundImage = url ? 'url(' + JSON.stringify(String(url)) + ')' : ''
        let mode = this.$v[Image.FILLMODE]
        let size = { [Stretch]: '100% 100%', [PreserveAspectFit]: 'contain', [PreserveAspectCrop]: 'cover', [Pad]: 'auto' }[mode] || 'auto'
        s.backgroundSize = size
        s.backgroundRepeat = mode === Tile ? 'repeat' : mode === TileVertically ? 'repeat-y' : mode === TileHorizontally ? 'repeat-x' : 'no-repeat'
        let h = this.$v[Image.HORIZONTALALIGNMENT], v = this.$v[Image.VERTICALALIGNMENT]
        s.backgroundPosition = (h === 1 ? 'left' : h === 2 ? 'right' : 'center') + ' ' + (v === 32 ? 'top' : v === 64 ? 'bottom' : 'center')
    }

    $disposeNative(){
        if(this.$loader) this.$loader.onload = this.$loader.onerror = null
        super.$disposeNative()
    }
}

const reload = (image) => image.$load()
const repaint = (image) => {
    image.$updatePainted()
    engine.markDirty(image, STYLE)
}

defineType(Image, {
    props: {
        source: { kind: 'string', effect: reload },
        fillMode: { kind: 'int', effect: repaint },
        sourceSize: { group: SourceSize },
        paintedWidth: 'real',
        paintedHeight: 'real',
        status: 'int',
        progress: 'real',
        asynchronous: 'bool',
        cache: { kind: 'bool', initial: true },
        smooth: { kind: 'bool', initial: true },
        mipmap: 'bool',
        mirror: { kind: 'bool', effect: repaint },
        mirrorVertically: 'bool',
        horizontalAlignment: { kind: 'int', initial: 4, effect: repaint },
        verticalAlignment: { kind: 'int', initial: 128, effect: repaint },
    },
})
Object.assign(Image, { AlignLeft: 0x1, AlignRight: 0x2, AlignHCenter: 0x4, AlignJustify: 0x8, AlignTop: 0x20, AlignBottom: 0x40, AlignVCenter: 0x80 })
Object.assign(Image, { Stretch, PreserveAspectFit, PreserveAspectCrop, Tile, TileVertically, TileHorizontally, Pad, Null, Ready, Loading, Error })

// painted size follows the item size
{
    let base = Image.prototype.$write
    Image.prototype.$write = function(id, value){
        base.call(this, id, value)
        if((id === Item.WIDTH || id === Item.HEIGHT) && this.$natural !== undefined) this.$updatePainted()
    }
}

module.exports = { SourceSize, Image }
