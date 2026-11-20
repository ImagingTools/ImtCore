const engine = require('../Core/engine')
const { GroupObject } = require('../Core/QtObject')
const { defineType, makeReadonly } = require('../Core/type')
const { Item, bits } = require('./Item')

const STYLE = bits.STYLE

const Stretch = 0, PreserveAspectFit = 1, PreserveAspectCrop = 2, Tile = 3, TileVertically = 4, TileHorizontally = 5, Pad = 6
const Null = 0, Ready = 1, Loading = 2, Error = 3

// Qt loads local files (and qrc) synchronously unless `asynchronous`, remote ones asynchronously; the page loads every
// image asynchronously. As Qt's QQuickPixmapCache an image the page has loaded is Ready at once; a new local source keeps
// the image it replaces (status, size, picture) until it is loaded, so no Loading state shows in between; a remote
// source goes through Loading as in Qt.
// As QQuickPixmapStore a url is loaded once: the Images of a url share its load, also while it is in progress, and
// draw what it loaded. An image of the page's own server is read once into a blob and drawn from it, so the browser
// does not ask the server again for every new element that shows it (icons come with Cache-Control: no-cache);
// another server's image is drawn from its url. An Image with cache: false loads its source on its own (QQuickPixmap::Cache)
const pixmaps = new Map()       // url -> { size: { width, height } | null, paint: url to draw, waiters: Set<Image> | null }
const isRemote = (source) => /^(https?|ftp):/i.test(String(source))
const nativeImage = () => typeof globalThis !== 'undefined' && globalThis.Image ? globalThis.Image : null

// the url of the page's own server, as a fetch and a blob can read it; null otherwise (another server, data:, no page)
function ownUrl(url){
    let g = globalThis
    if(typeof g.fetch !== 'function' || !g.URL || typeof g.URL.createObjectURL !== 'function' || !g.location || !g.location.origin) return null
    try {
        let absolute = new g.URL(url, g.location.href)
        return absolute.origin === g.location.origin && /^https?:$/.test(absolute.protocol) ? absolute.href : null
    } catch(error) {
        return null
    }
}

// loads url into an image element and reads its size; the blob of an own url is read first and the element shows it
function readImage(url, done){
    let Native = nativeImage()
    let decode = (src, paint) => {
        let loader = new Native()
        loader.onload = () => done({ width: loader.naturalWidth, height: loader.naturalHeight }, paint)
        loader.onerror = () => {
            if(paint !== url) globalThis.URL.revokeObjectURL(paint)
            done(null, url)
        }
        loader.src = src
    }
    let own = ownUrl(url)
    if(!own) return decode(url, url)
    globalThis.fetch(own).then((response) => {
        if(!response.ok) throw new Error(String(response.status))
        return response.blob()
    }).then((blob) => {
        let paint = globalThis.URL.createObjectURL(blob)
        decode(paint, paint)
    }, () => done(null, url))
}

function loadPixmap(url, image){
    let entry = pixmaps.get(url)
    if(entry){
        if(entry.waiters) entry.waiters.add(image)
        return entry
    }
    entry = { size: null, paint: url, waiters: new Set([image]) }
    pixmaps.set(url, entry)
    readImage(url, (size, paint) => engine.batch(() => {
        let waiters = entry.waiters
        entry.waiters = null
        entry.size = size
        entry.paint = paint
        if(!size) pixmaps.delete(url)       // a failed load is tried again by the next Image of the url
        for(let waiting of waiters) waiting.$loaded(url, entry)
    }))
    return entry
}
// sourceSize: a QSize value type of width and height; -1 is a part not set.
// QQuickImageBase::sourceSize reads a part not set as the size of the loaded picture (0 without one). Writing one part is
// a write of the whole QSize read first: the other part not set takes its value read back (sourceSize.width: 10 before
// the picture loads leaves the height 0). A change emits sourceSizeChanged of the image
class SourceSize extends GroupObject {
    $write(id, value){
        let other = id === SourceSize.WIDTH ? SourceSize.HEIGHT : SourceSize.WIDTH
        if(this.$v[other] === -1) this.$v[other] = this.$readBack(other)
        super.$write(id, value)
    }

    $readBack(id){
        let value = this.$v[id]
        if(value !== -1) return value
        let natural = this.$owner.$natural
        return natural ? (id === SourceSize.WIDTH ? natural.width : natural.height) : 0
    }
}
const sourceSizeChanged = (size) => {
    size.$owner.$updateSize()
    engine.emit(size.$owner, Image.SIGNAL_sourceSizeChanged)
}
const readBack = (name) => function(){
    let id = SourceSize[name.toUpperCase()]
    engine.track(this, SourceSize.$type.props[id].signal)
    return this.$readBack(id)
}
defineType(SourceSize, {
    props: {
        width: { kind: 'real', initial: -1, effect: sourceSizeChanged, get: readBack('width') },
        height: { kind: 'real', initial: -1, effect: sourceSizeChanged, get: readBack('height') },
    },
})

// QQuickImageProviderWithOptions::loadSize: the size a picture of originalSize is loaded at for the requested size (0 is a
// part not requested). A raster picture is only made smaller, a vector one (svg) takes the size; with PreserveAspectFit
// or PreserveAspectCrop the picture covers the box. Nothing requested: null (the natural size)
function loadSize(original, requested, scalable, cropOrFit){
    let rw = requested.width, rh = requested.height
    if((rw <= 0 && rh <= 0) || original.width <= 0 || original.height <= 0) return null
    if(!cropOrFit && scalable && rw > 0 && rh > 0) return { width: rw, height: rh }
    let ratio = 0
    if(rw > 0 && (cropOrFit || scalable || rw < original.width)) ratio = rw / original.width
    if(rh > 0 && (cropOrFit || scalable || rh < original.height)){
        let hr = rh / original.height
        if(ratio === 0) ratio = hr
        else if(!cropOrFit && hr < ratio) ratio = hr
        else if(cropOrFit && hr > ratio) ratio = hr
    }
    return ratio > 0 ? { width: Math.round(original.width * ratio), height: Math.round(original.height * ratio) } : null
}

const isScalable = (url) => /\.svgz?$/i.test(String(url || '').replace(/[?#].*$/, ''))

class Image extends Item {
    constructor(parent = null){
        super(parent)
        this.$pending = null      // the pixmap entry this Image waits for
        this.$natural = null      // { width, height } of the loaded image
        this.$paintUrl = ''       // what the element draws: the blob of an own url, or the url
    }

    $stopWaiting(){
        let pending = this.$pending
        this.$pending = null
        if(pending && pending.waiters) pending.waiters.delete(this)
    }

    $load(){
        let source = this.$v[Image.SOURCE]
        this.$stopWaiting()
        if(!source){
            this.$natural = null
            this.$url = this.$paintUrl = ''
            this.$write(Image.PROGRESS, 0)
            this.$write(Image.STATUS, Null)
            this.$updateSize()
            return
        }
        if(!nativeImage()) return
        let url = require('../Core/Resources').resolveUrl(source)
        let cached = this.$v[Image.CACHE] ? pixmaps.get(url) : null
        if(cached && cached.size){
            this.$ready(url, cached.size, cached.paint)
            return
        }
        let keep = !isRemote(source) && this.$v[Image.STATUS] === Ready && !this.$v[Image.ASYNCHRONOUS]
        if(!keep){
            this.$natural = null
            this.$url = url
            this.$paintUrl = ''         // nothing is drawn until it is loaded
            this.$updateSize()
            this.$write(Image.PROGRESS, 0)
            this.$write(Image.STATUS, Loading)
        }
        if(this.$v[Image.CACHE]){
            this.$pending = loadPixmap(url, this)
            return
        }
        let entry = { size: null, paint: url, waiters: new Set([this]) }
        this.$pending = entry
        readImage(url, (size, paint) => engine.batch(() => {
            entry.waiters = null
            entry.size = size
            entry.paint = paint
            this.$loaded(url, entry)
        }))
    }

    // the load of url this Image waits for has ended
    $loaded(url, entry){
        if(this.$pending !== entry) return
        this.$pending = null
        if(entry.size){
            this.$ready(url, entry.size, entry.paint)
            return
        }
        engine.reportError('Cannot open: ' + this.$v[Image.SOURCE], 'Image')
        this.$natural = null
        this.$url = url
        this.$paintUrl = ''
        this.$updateSize()
        this.$write(Image.STATUS, Error)
    }

    $ready(url, size, paint){
        let group = this.$v[Image.SOURCESIZE]
        let before = group ? [group.$readBack(SourceSize.WIDTH), group.$readBack(SourceSize.HEIGHT)] : null
        this.$natural = { width: size.width, height: size.height }
        // QQuickImageBase::requestFinished: sourceSizeChanged when the size read back changes
        if(group && (group.$readBack(SourceSize.WIDTH) !== before[0] || group.$readBack(SourceSize.HEIGHT) !== before[1])){
            engine.emit(group, SourceSize.SIGNAL_widthChanged)
            engine.emit(group, SourceSize.SIGNAL_heightChanged)
            engine.emit(this, Image.SIGNAL_sourceSizeChanged)
        }
        this.$url = url
        this.$paintUrl = paint
        this.$updateSize()
        this.$write(Image.PROGRESS, 1)
        this.$write(Image.STATUS, Ready)
    }

    // implicit size: the loaded size scaled to sourceSize (one dimension keeps the aspect ratio)
    $updateSize(){
        let natural = this.$natural
        let width = 0, height = 0
        if(natural){
            width = natural.width
            height = natural.height
            // the values themselves: a binding being evaluated when the size updates does not depend on them
            let group = this.$v[Image.SOURCESIZE]
            if(group){
                let mode = this.$v[Image.FILLMODE]
                let size = loadSize(natural, { width: group.$v[SourceSize.WIDTH], height: group.$v[SourceSize.HEIGHT] },
                    isScalable(this.$url), mode === PreserveAspectFit || mode === PreserveAspectCrop)
                if(size){
                    width = size.width
                    height = size.height
                }
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
        let url = this.$paintUrl || ''
        s.backgroundImage = url ? 'url(' + JSON.stringify(String(url)) + ')' : ''
        let mode = this.$v[Image.FILLMODE]
        let size = { [Stretch]: '100% 100%', [PreserveAspectFit]: 'contain', [PreserveAspectCrop]: 'cover', [Pad]: 'auto' }[mode] || 'auto'
        s.backgroundSize = size
        s.backgroundRepeat = mode === Tile ? 'repeat' : mode === TileVertically ? 'repeat-y' : mode === TileHorizontally ? 'repeat-x' : 'no-repeat'
        let h = this.$v[Image.HORIZONTALALIGNMENT], v = this.$v[Image.VERTICALALIGNMENT]
        s.backgroundPosition = (h === 1 ? 'left' : h === 2 ? 'right' : 'center') + ' ' + (v === 32 ? 'top' : v === 64 ? 'bottom' : 'center')
    }

    $disposeNative(){
        this.$stopWaiting()
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
        // the fill mode decides the size a picture is loaded at too (preserveAspectRatioCrop / Fit)
        fillMode: { kind: 'int', effect: (image) => image.$updateSize() },
        sourceSize: { group: SourceSize },
        paintedWidth: { kind: 'real', readonly: true },
        paintedHeight: { kind: 'real', readonly: true },
        status: { kind: 'int', readonly: true },
        progress: { kind: 'real', readonly: true },
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

// read-only in Qt, writable in the base class
makeReadonly(Image, 'implicitWidth')
makeReadonly(Image, 'implicitHeight')

module.exports = { SourceSize, Image }
