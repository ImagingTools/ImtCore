// Item.layer (QQuickItemLayer), MultiEffect of QtQuick.Effects (qquickmultieffect.cpp, multieffect.frag, Qt 6.12) and
// ShaderEffectSource. A layer with an effect is drawn as CSS on the item's DOM:
//  - MultiEffect: contrast, brightness, colorization and saturation as one SVG color matrix, as multieffect.frag
//    computes them (color.rgb = (rgb - 0.5) * (1 + contrast) + 0.5 + brightness; gray = dot(rgb, 0.299 0.587 0.114);
//    rgb = gray * colorizationColor.rgb * a + rgb * (1 - a), a = colorizationColor.a * colorization; rgb = mix(gray, rgb,
//    1 + saturation)); blur and shadow as CSS blur() and drop-shadow() (their radius from blurMax); a mask of a
//    ShaderEffectSource of a Rectangle as a rounded clip, of an Image as a CSS mask;
//  - an item with a layer and no effect is drawn as it is.
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { QtObject, GroupObject } = require('../Core/QtObject')
const { Item } = require('./Item')

let filterCount = 0
// the SVG of the filters of the page
function filterRoot(){
    if(typeof document === 'undefined') return null
    let svg = document.getElementById('jq-effects')
    if(!svg){
        svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg')
        svg.id = 'jq-effects'
        svg.setAttribute('width', '0')
        svg.setAttribute('height', '0')
        svg.style.position = 'absolute'
        svg.style.pointerEvents = 'none'
        svg.appendChild(document.createElementNS('http://www.w3.org/2000/svg', 'defs'))
        document.body.appendChild(svg)
    }
    return svg.firstChild
}

// a color as 0 - 1 channels (parseColor: 16 bits each)
const rgba = (c) => {
    let { parseColor } = require('../Core/color')
    let p = parseColor(String(c || '')) || { a: 65535, r: 0, g: 0, b: 0 }
    return { a: p.a / 65535, r: p.r / 65535, g: p.g / 65535, b: p.b / 65535 }
}

// ---- MultiEffect (QQuickMultiEffect) ----
class MultiEffect extends Item {
    constructor(parent = null){
        super(parent)
        let defaults = { COLORIZATIONCOLOR: '#ff0000', BLURMAX: 32, SHADOWOPACITY: 1, SHADOWBLUR: 1, SHADOWCOLOR: '#000000',
            SHADOWSCALE: 1, AUTOPADDINGENABLED: true, MASKTHRESHOLDMAX: 1 }
        for(let k in defaults) QtObject.prototype.$write.call(this, MultiEffect[k], defaults[k])
        this.$owners = new Set()
    }
    $write(id, value){
        super.$write(id, value)
        if(id >= MultiEffect.SOURCE) for(let owner of this.$owners) applyLayer(owner)
    }
    // the 4 x 5 color matrix of multieffect.frag (unpremultiplied: the same formulas)
    $matrix(){
        let v = this.$v
        let k = v[MultiEffect.CONTRAST], b = v[MultiEffect.BRIGHTNESS], s = v[MultiEffect.SATURATION]
        let c = rgba(v[MultiEffect.COLORIZATIONCOLOR])
        let a = Math.max(0, Math.min(1, c.a * v[MultiEffect.COLORIZATION]))
        if(!k && !b && !s && !a) return null
        // contrast and brightness: rgb' = rgb * (1 + k) - 0.5 k + b
        let bc = (i) => { let row = [0, 0, 0, 0, 0]; row[i] = 1 + k; row[4] = -0.5 * k + b; return row }
        let m = [bc(0), bc(1), bc(2), [0, 0, 0, 1, 0]]
        const mul = (A, B) => A.map(row => {
            let out = [0, 0, 0, 0, 0]
            for(let j = 0; j < 5; j++){
                for(let i = 0; i < 4; i++) out[j] += row[i] * B[i][j]
                if(j === 4) out[4] += row[4]
            }
            return out
        })
        const W = [0.299, 0.587, 0.114]
        // colorization: rgb = gray * C * a + rgb * (1 - a)
        let C = [c.r, c.g, c.b]
        let col = [0, 1, 2].map(i => { let row = W.map(w => w * C[i] * a).concat([0, 0]); row[i] += 1 - a; return row })
        col.push([0, 0, 0, 1, 0])
        m = mul(col, m)
        // saturation: rgb = gray + (rgb - gray) * (1 + s)
        let f = 1 + s
        let sat = [0, 1, 2].map(i => { let row = W.map(w => w * (1 - f)).concat([0, 0]); row[i] += f; return row })
        sat.push([0, 0, 0, 1, 0])
        m = mul(sat, m)
        return m
    }
    // the CSS of the effect on the item it is the layer of
    $css(owner){
        let v = this.$v
        let filters = []
        let matrix = this.$matrix()
        if(matrix){
            let defs = filterRoot()
            if(defs){
                if(!this.$filter){
                    let ns = 'http://www.w3.org/2000/svg'
                    this.$filter = document.createElementNS(ns, 'filter')
                    this.$filter.id = 'jq-fx-' + (++filterCount)
                    this.$filter.setAttribute('color-interpolation-filters', 'sRGB')
                    let cm = document.createElementNS(ns, 'feColorMatrix')
                    cm.setAttribute('type', 'matrix')
                    this.$filter.appendChild(cm)
                    defs.appendChild(this.$filter)
                }
                this.$filter.firstChild.setAttribute('values', matrix.map(row => row.join(' ')).join(' '))
                filters.push('url(#' + this.$filter.id + ')')
            }
        }
        let blurMax = v[MultiEffect.BLURMAX] * (1 + v[MultiEffect.BLURMULTIPLIER])
        if(v[MultiEffect.BLURENABLED] && v[MultiEffect.BLUR] > 0) filters.push('blur(' + (v[MultiEffect.BLUR] * blurMax / 2) + 'px)')
        if(v[MultiEffect.SHADOWENABLED]){
            let c = rgba(v[MultiEffect.SHADOWCOLOR])
            let alpha = c.a * Math.max(0, Math.min(1, v[MultiEffect.SHADOWOPACITY]))
            let color = 'rgba(' + Math.round(c.r * 255) + ',' + Math.round(c.g * 255) + ',' + Math.round(c.b * 255) + ',' + alpha + ')'
            filters.push('drop-shadow(' + v[MultiEffect.SHADOWHORIZONTALOFFSET] + 'px ' + v[MultiEffect.SHADOWVERTICALOFFSET] + 'px ' + (v[MultiEffect.SHADOWBLUR] * blurMax / 2) + 'px ' + color + ')')
        }
        let css = { filter: filters.join(' '), clipPath: '', mask: '' }
        // a mask: a Rectangle (in a ShaderEffectSource) clips round, an Image is a mask image
        let mask = v[MultiEffect.MASKENABLED] && !v[MultiEffect.MASKINVERTED] ? v[MultiEffect.MASKSOURCE] : null
        let item = mask && mask.sourceItem !== undefined ? mask.sourceItem : mask
        if(item && item.$isItem){
            if(item.radius !== undefined){
                let ox = owner.$v[Item.X], oy = owner.$v[Item.Y]
                let top = Math.max(0, item.y - oy), left = Math.max(0, item.x - ox)
                let right = Math.max(0, owner.width - left - item.width), bottom = Math.max(0, owner.height - top - item.height)
                // (the source item of the gallery is placed by its own x and y, in the coordinates of the owner's parent)
                if(item.x === 0 && item.y === 0){ top = 0; left = 0; right = Math.max(0, owner.width - item.width); bottom = Math.max(0, owner.height - item.height) }
                css.clipPath = 'inset(' + top + 'px ' + right + 'px ' + bottom + 'px ' + left + 'px round ' + item.radius + 'px)'
            } else if(item.source){
                css.mask = 'url("' + item.source + '") center / contain no-repeat'
            }
        }
        return css
    }
    $disposeNative(){
        if(this.$filter && this.$filter.parentNode) this.$filter.parentNode.removeChild(this.$filter)
        this.$filter = null
        super.$disposeNative()
    }
}
defineType(MultiEffect, {
    props: {
        source: { kind: 'var', initial: null }, autoPaddingEnabled: 'bool', paddingRect: 'var',
        brightness: 'real', contrast: 'real', saturation: 'real', colorization: 'real', colorizationColor: 'color',
        blurEnabled: 'bool', blur: 'real', blurMax: 'int', blurMultiplier: 'real',
        shadowEnabled: 'bool', shadowOpacity: 'real', shadowBlur: 'real', shadowHorizontalOffset: 'real', shadowVerticalOffset: 'real',
        shadowColor: 'color', shadowScale: 'real',
        maskEnabled: 'bool', maskSource: { kind: 'var', initial: null }, maskThresholdMin: 'real', maskSpreadAtMin: 'real',
        maskThresholdMax: 'real', maskSpreadAtMax: 'real', maskInverted: 'bool',
        itemRect: { kind: 'var', readonly: true }, fragmentShader: { kind: 'string', readonly: true },
        vertexShader: { kind: 'string', readonly: true }, hasProxySource: { kind: 'bool', readonly: true },
    },
    signals: ['shaderChanged', 'itemSizeChanged'],
})

// ---- ShaderEffectSource (QQuickShaderEffectSource): a texture of an item; here what a mask or a layer reads ----
class ShaderEffectSource extends Item {
    constructor(parent = null){
        super(parent)
        QtObject.prototype.$write.call(this, ShaderEffectSource.LIVE, true)
        QtObject.prototype.$write.call(this, ShaderEffectSource.SMOOTH, true)
    }
    scheduleUpdate(){}
}
defineType(ShaderEffectSource, {
    props: {
        sourceItem: { kind: 'var', initial: null }, sourceRect: 'var', textureSize: 'var', format: 'int', live: 'bool',
        hideSource: 'bool', mipmap: 'bool', recursive: 'bool', wrapMode: 'int', textureMirroring: 'int', samples: 'int',
    },
    signals: ['scheduledUpdateCompleted'],
})
Object.assign(ShaderEffectSource, { ClampToEdge: 0, RepeatHorizontally: 1, RepeatVertically: 2, Repeat: 3,
    Alpha: 0x6406, RGB: 0x1907, RGBA: 0x1908, RGBA8: 1, RGBA16F: 2, RGBA32F: 3, NoMirroring: 0, MirrorHorizontally: 1, MirrorVertically: 2 })

// ---- Item.layer (QQuickItemLayer) ----
class ItemLayer extends GroupObject {
    constructor(owner){
        super(owner)
        QtObject.prototype.$write.call(this, ItemLayer.SMOOTH, false)
        QtObject.prototype.$write.call(this, ItemLayer.SAMPLERNAME, 'source')
    }
    $write(id, value){
        super.$write(id, value)
        if(id === ItemLayer.ENABLED || id === ItemLayer.EFFECT) updateLayer(this.$owner)
    }
}
defineType(ItemLayer, {
    props: {
        enabled: 'bool', effect: { kind: 'component', initial: null }, samplerName: 'string', smooth: 'bool', mipmap: 'bool',
        format: 'int', sourceRect: 'var', textureSize: 'var', wrapMode: 'int', samples: 'int', textureMirroring: 'int', live: 'bool',
    },
})

// the effect of an item's layer: made when the layer is enabled, its source the item (QQuickItemLayer::activateEffect)
function updateLayer(item){
    let layer = item.$v[Item.LAYER]
    let enabled = layer && layer.$v[ItemLayer.ENABLED]
    let component = layer && layer.$v[ItemLayer.EFFECT]
    if(item.$layerEffect && (!enabled || item.$layerComponent !== component)){
        let old = item.$layerEffect
        item.$layerEffect = null
        old.$owners.delete(item)
        old.destroy()
    }
    if(enabled && component && !item.$layerEffect && component.createObject){
        let effect = component.createObject(null)
        if(effect){
            item.$layerEffect = effect
            item.$layerComponent = component
            if(effect.$owners) effect.$owners.add(item)
            let prop = effect.constructor.$type.propByName.get(layer.$v[ItemLayer.SAMPLERNAME] || 'source')
            if(prop) effect.$write(prop.id, item)
            if(effect.$v[Item.VISIBLE] !== undefined) effect.$write(Item.VISIBLE, false)
            // the effect follows the size of the item
            effect.$bind(Item.WIDTH, () => item.width, null, effect)
            effect.$bind(Item.HEIGHT, () => item.height, null, effect)
            let reapply = () => applyLayer(item)
            engine.connect(item, Item.SIGNAL_widthChanged, reapply, null, engine.PRIORITY_NORMAL, effect)
            engine.connect(item, Item.SIGNAL_heightChanged, reapply, null, engine.PRIORITY_NORMAL, effect)
        }
    }
    applyLayer(item)
}
function applyLayer(item){
    let dom = item.$dom
    if(!dom) return
    let effect = item.$layerEffect
    let css = effect && effect.$css ? effect.$css(item) : { filter: '', clipPath: '', mask: '' }
    dom.style.filter = css.filter
    dom.style.clipPath = css.clipPath
    dom.style.mask = css.mask
    dom.style.webkitMask = css.mask
}

{
    let prop = Item.$type.propByName.get('layer')
    prop.group = ItemLayer
    Object.defineProperty(Item.prototype, 'layer', {
        get(){ return this.$v[prop.id] || (this.$v[prop.id] = new ItemLayer(this)) },
        configurable: true,
    })
}

module.exports = { MultiEffect, ShaderEffectSource, ItemLayer }
