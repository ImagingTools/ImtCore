// Qt color values, normalized like QtQml/Color.js of the current runtime:
// #rrggbb, or #aarrggbb with the alpha first; toCss converts for styles.
const NAMED = {"aliceblue":"#f0f8ff","grey":"#808080","darkgrey":"#a9a9a9","dimgrey":"#696969","slategrey":"#708090","lightslategrey":"#778899","darkslategrey":"#2f4f4f","antiquewhite":"#faebd7","aqua":"#00ffff","aquamarine":"#7fffd4","azure":"#f0ffff","beige":"#f5f5dc","bisque":"#ffe4c4","black":"#000000","blanchedalmond":"#ffebcd","blue":"#0000ff","blueviolet":"#8a2be2","brown":"#a52a2a","burlywood":"#deb887","cadetblue":"#5f9ea0","chartreuse":"#7fff00","chocolate":"#d2691e","coral":"#ff7f50","cornflowerblue":"#6495ed","cornsilk":"#fff8dc","crimson":"#dc143c","cyan":"#00ffff","darkblue":"#00008b","darkcyan":"#008b8b","darkgoldenrod":"#b8860b","darkgray":"#a9a9a9","darkgreen":"#006400","darkkhaki":"#bdb76b","darkmagenta":"#8b008b","darkolivegreen":"#556b2f","darkorange":"#ff8c00","darkorchid":"#9932cc","darkred":"#8b0000","darksalmon":"#e9967a","darkseagreen":"#8fbc8f","darkslateblue":"#483d8b","darkslategray":"#2f4f4f","darkturquoise":"#00ced1","darkviolet":"#9400d3","deeppink":"#ff1493","deepskyblue":"#00bfff","dimgray":"#696969","dodgerblue":"#1e90ff","firebrick":"#b22222","floralwhite":"#fffaf0","forestgreen":"#228b22","fuchsia":"#ff00ff","gainsboro":"#dcdcdc","ghostwhite":"#f8f8ff","gold":"#ffd700","goldenrod":"#daa520","gray":"#808080","green":"#008000","greenyellow":"#adff2f","honeydew":"#f0fff0","hotpink":"#ff69b4","indianred":"#cd5c5c","indigo":"#4b0082","ivory":"#fffff0","khaki":"#f0e68c","lavender":"#e6e6fa","lavenderblush":"#fff0f5","lawngreen":"#7cfc00","lemonchiffon":"#fffacd","lightblue":"#add8e6","lightcoral":"#f08080","lightcyan":"#e0ffff","lightgoldenrodyellow":"#fafad2","lightgrey":"#d3d3d3","lightgray":"#d3d3d3","lightgreen":"#90ee90","lightpink":"#ffb6c1","lightsalmon":"#ffa07a","lightseagreen":"#20b2aa","lightskyblue":"#87cefa","lightslategray":"#778899","lightsteelblue":"#b0c4de","lightyellow":"#ffffe0","lime":"#00ff00","limegreen":"#32cd32","linen":"#faf0e6","magenta":"#ff00ff","maroon":"#800000","mediumaquamarine":"#66cdaa","mediumblue":"#0000cd","mediumorchid":"#ba55d3","mediumpurple":"#9370db","mediumseagreen":"#3cb371","mediumslateblue":"#7b68ee","mediumspringgreen":"#00fa9a","mediumturquoise":"#48d1cc","mediumvioletred":"#c71585","midnightblue":"#191970","mintcream":"#f5fffa","mistyrose":"#ffe4e1","moccasin":"#ffe4b5","navajowhite":"#ffdead","navy":"#000080","oldlace":"#fdf5e6","olive":"#808000","olivedrab":"#6b8e23","orange":"#ffa500","orangered":"#ff4500","orchid":"#da70d6","palegoldenrod":"#eee8aa","palegreen":"#98fb98","paleturquoise":"#afeeee","palevioletred":"#db7093","papayawhip":"#ffefd5","peachpuff":"#ffdab9","peru":"#cd853f","pink":"#ffc0cb","plum":"#dda0dd","powderblue":"#b0e0e6","purple":"#800080","rebeccapurple":"#663399","red":"#ff0000","rosybrown":"#bc8f8f","royalblue":"#4169e1","saddlebrown":"#8b4513","salmon":"#fa8072","sandybrown":"#f4a460","seagreen":"#2e8b57","seashell":"#fff5ee","sienna":"#a0522d","silver":"#c0c0c0","skyblue":"#87ceeb","slateblue":"#6a5acd","slategray":"#708090","snow":"#fffafa","springgreen":"#00ff7f","steelblue":"#4682b4","tan":"#d2b48c","teal":"#008080","thistle":"#d8bfd8","tomato":"#ff6347","turquoise":"#40e0d0","violet":"#ee82ee","wheat":"#f5deb3","white":"#ffffff","whitesmoke":"#f5f5f5","yellow":"#ffff00","yellowgreen":"#9acd32"}

function coerceColor(value){
    if(value === undefined) throw 'Cannot assign [undefined] to color'
    if(value === null || typeof value === 'number' || typeof value === 'boolean') return '#000000'
    if(typeof value !== 'string') throw 'Cannot assign QJSValue to color'
    if(value === '') return '#000000'
    let lower = value.toLowerCase()
    if(lower === 'transparent') return '#00000000'
    if(NAMED[lower]) return NAMED[lower]
    if(lower[0] === '#'){
        if(lower.length === 4) return '#' + lower[1] + lower[1] + lower[2] + lower[2] + lower[3] + lower[3]
        if(lower.length === 5) return '#' + lower[1] + lower[1] + lower[2] + lower[2] + lower[3] + lower[3] + lower[4] + lower[4]
        if(lower.length === 7 || lower.length === 9) return lower
    }
    return '#000000'
}

// Qt writes alpha first (#AARRGGBB), CSS last (#RRGGBBAA): an 8-digit value goes to styles as rgba()
function toCss(value){
    let hex = value === undefined || value === null ? '#000000' : coerceColor(String(value))
    if(hex.length === 9){
        return 'rgba(' + parseInt(hex.slice(3, 5), 16) + ',' + parseInt(hex.slice(5, 7), 16) + ',' + parseInt(hex.slice(7, 9), 16) + ',' + parseInt(hex.slice(1, 3), 16) / 255 + ')'
    }
    return hex
}

// Qt.rgba(r, g, b, a) with components 0..1
function rgba(r, g, b, a = 1){
    let byte = (v) => Math.max(0, Math.min(255, Math.round(v * 255))).toString(16).padStart(2, '0')
    return a >= 1 ? '#' + byte(r) + byte(g) + byte(b) : '#' + byte(a) + byte(r) + byte(g) + byte(b)
}

// ---- QColor arithmetic (qcolor.cpp of Qt 6.12, qquickglobal.cpp QQuickColorProvider) --------------------------------------
// A color is 16 bits a component, as QColor keeps it; the float math of Qt is done in float (Math.fround), so the
// results print as Qt prints them. A color value of this runtime is its name (#rrggbb, #aarrggbb).

const F = Math.fround
const USHRT_MAX = 65535
const qRound = (d) => (d >= 0 ? Math.trunc(F(d + 0.5)) : Math.trunc(F(d - 0.5)))
// a * b + c in float, each operation rounded (12 000 values of lighter / darker / hsla / hsva agree with Qt 6.12 on
// arm64 this way, as with a fused multiply-add)
const fma = (a, b, c) => F(F(a * b) + c)
// qRound(x * k): the product is rounded to a float before qRound adds 0.5f (contraction stays within an expression)
const roundMul = (x, k) => qRound(F(x * k))
const div257 = (x) => (x - ((x + 128) >> 8) + 128) >> 8

function hexInt(text, from, digits){
    let v = parseInt(text.substr(from, digits), 16)
    return Number.isNaN(v) ? -1 : v
}

// QColor::fromString: #RGB, #RRGGBB, #AARRGGBB, #RRRGGGBBB, #RRRRGGGGBBBB, an SVG color name, "transparent";
// { a, r, g, b } of 16 bits, or null when it is no color
function parseColor(value){
    if(typeof value !== 'string') return null
    let text = value.trim().toLowerCase()
    if(text === 'transparent') return { a: 0, r: 0, g: 0, b: 0 }
    if(NAMED[text]) text = NAMED[text]
    if(text[0] !== '#' || !/^#[0-9a-f]+$/.test(text)) return null
    let n = text.length - 1
    let w8 = (v) => v * 0x101
    if(n === 3) return { a: USHRT_MAX, r: w8(hexInt(text, 1, 1) * 0x11), g: w8(hexInt(text, 2, 1) * 0x11), b: w8(hexInt(text, 3, 1) * 0x11) }
    if(n === 6) return { a: USHRT_MAX, r: w8(hexInt(text, 1, 2)), g: w8(hexInt(text, 3, 2)), b: w8(hexInt(text, 5, 2)) }
    if(n === 8) return { a: w8(hexInt(text, 1, 2)), r: w8(hexInt(text, 3, 2)), g: w8(hexInt(text, 5, 2)), b: w8(hexInt(text, 7, 2)) }
    if(n === 9){
        let w12 = (v) => (v << 4) | (v >> 8)
        return { a: USHRT_MAX, r: w12(hexInt(text, 1, 3)), g: w12(hexInt(text, 4, 3)), b: w12(hexInt(text, 7, 3)) }
    }
    if(n === 12) return { a: USHRT_MAX, r: hexInt(text, 1, 4), g: hexInt(text, 5, 4), b: hexInt(text, 9, 4) }
    return null
}

// QVariant(QColor).toString(): name(HexRgb) when opaque, name(HexArgb) otherwise
function colorName(c){
    let hex = (v) => div257(v).toString(16).padStart(2, '0')
    return (div257(c.a) === 255 ? '#' : '#' + hex(c.a)) + hex(c.r) + hex(c.g) + hex(c.b)
}

// QColor::toHsv of an Rgb color (Qt 6.12: on the 16-bit integers): { a, h (hundredths of a degree, USHRT_MAX
// achromatic), s, v }
function rgbToHsv(c){
    const r = c.r, g = c.g, b = c.b
    const max = Math.max(r, g, b), min = Math.min(r, g, b)
    let hsv = { a: c.a, h: USHRT_MAX, s: 0, v: max }
    if(max === min) return hsv
    const chroma = F(max - min)
    hsv.s = roundMul(F(chroma / max), USHRT_MAX)
    let hue
    if(max === r){
        hue = F(F(g - b) / chroma)
        if(hue < 0) hue = F(hue + 6)
    } else if(max === g){
        hue = F(2 + F(F(b - r) / chroma))
    } else {
        hue = F(4 + F(F(r - g) / chroma))
    }
    hsv.h = roundMul(hue, 6000)
    return hsv
}

// QColor::toRgb of an Hsv color
function hsvToRgb(c){
    let rgb = { a: c.a, r: c.v, g: c.v, b: c.v }
    if(c.s === 0 || c.h === USHRT_MAX) return rgb
    const h = c.h === 36000 ? 0 : F(c.h / 6000)
    const s = F(c.s / USHRT_MAX), v = F(c.v / USHRT_MAX)
    const i = Math.trunc(h)
    const f = F(h - i)
    const p = F(v * F(1 - s))
    const q16 = (x) => roundMul(x, USHRT_MAX)
    if(i & 1){
        const q = F(v * fma(-s, f, 1))
        if(i === 1) Object.assign(rgb, { r: q16(q), g: q16(v), b: q16(p) })
        else if(i === 3) Object.assign(rgb, { r: q16(p), g: q16(q), b: q16(v) })
        else Object.assign(rgb, { r: q16(v), g: q16(p), b: q16(q) })
    } else {
        const t = F(v * fma(-s, F(1 - f), 1))
        if(i === 0) Object.assign(rgb, { r: q16(v), g: q16(t), b: q16(p) })
        else if(i === 2) Object.assign(rgb, { r: q16(p), g: q16(v), b: q16(t) })
        else Object.assign(rgb, { r: q16(t), g: q16(p), b: q16(v) })
    }
    return rgb
}

// QColor::toRgb of an Hsl color
function hslToRgb(c){
    let rgb = { a: c.a, r: c.l, g: c.l, b: c.l }
    if(c.s === 0 || c.h === USHRT_MAX) return rgb
    if(c.l === 0) return Object.assign(rgb, { r: 0, g: 0, b: 0 })
    const h = c.h === 36000 ? 0 : F(c.h / 36000)
    const s = F(c.s / USHRT_MAX), l = F(c.l / USHRT_MAX)
    const temp2 = l < 0.5 ? F(l * F(1 + s)) : fma(-l, s, F(l + s))
    const temp1 = fma(2, l, -temp2)
    const third = F(1 / 3)
    const temp3 = [F(h + third), h, F(h - third)]
    const out = []
    for(let i = 0; i < 3; i++){
        let t = temp3[i]
        if(t < 0) t = F(t + 1)
        else if(t > 1) t = F(t - 1)
        const six = F(t * 6)
        let x
        if(six < 1) x = fma(F(temp2 - temp1), six, temp1)
        else if(F(t * 2) < 1) x = temp2
        else if(F(t * 3) < 2) x = fma(F(F(temp2 - temp1) * F(F(F(2) / 3) - t)), 6, temp1)
        else x = temp1
        let v = roundMul(x, USHRT_MAX)
        out.push(v === 1 ? 0 : v)
    }
    return Object.assign(rgb, { r: out[0], g: out[1], b: out[2] })
}

const clamp01 = (x) => Math.max(0, Math.min(1, x))

// Qt.rgba: QColor::fromRgbF of the clamped components
function rgbaColor(r, g, b, a = 1){
    const q16 = (x) => roundMul(F(clamp01(Number(x))), USHRT_MAX)
    return colorName({ a: q16(a), r: q16(r), g: q16(g), b: q16(b) })
}

// Qt.hsla / Qt.hsva: QColor::fromHslF / fromHsvF (hue -1: achromatic), printed as its Rgb value
function hslaColor(h, s, l, a = 1){
    h = Number(h)
    if(h !== -1) h = clamp01(h)
    let hue = h === -1 ? USHRT_MAX : roundMul(F(h), 36000)
    if(hue === 36000) hue = 0
    const q16 = (x) => roundMul(F(clamp01(Number(x))), USHRT_MAX)
    return colorName(hslToRgb({ a: q16(a), h: hue, s: q16(s), l: q16(l) }))
}

function hsvaColor(h, s, v, a = 1){
    h = Number(h)
    if(h !== -1) h = clamp01(h)
    const q16 = (x) => roundMul(F(clamp01(Number(x))), USHRT_MAX)
    return colorName(hsvToRgb({ a: q16(a), h: h === -1 ? USHRT_MAX : roundMul(F(h), 36000), s: q16(s), v: q16(v) }))
}

// QColor::lighter / darker (the factor is a percentage: qRound(factor * 100))
function lighter16(c, factor){
    if(factor <= 0) return c
    if(factor < 100) return darker16(c, Math.trunc(10000 / factor))
    let hsv = rgbToHsv(c)
    let s = hsv.s
    let v = Math.floor((factor * hsv.v) / 100) >>> 0
    if(v > USHRT_MAX){
        s -= v - USHRT_MAX
        if(s < 0) s = 0
        v = USHRT_MAX
    }
    return hsvToRgb(Object.assign(hsv, { s, v }))
}

function darker16(c, factor){
    if(factor <= 0) return c
    if(factor < 100) return lighter16(c, Math.trunc(10000 / factor))
    let hsv = rgbToHsv(c)
    hsv.v = Math.floor((hsv.v * 100) / factor)
    return hsvToRgb(hsv)
}

// QtObject::colorVariantFromJSValue: a color or a string that names one; anything else is returned as it is
function colorArgument(value){
    return typeof value === 'string' ? parseColor(value) : null
}

const percent = (factor) => Math.trunc(qRound(Number(factor) * 100))

function lighterColor(color, factor = 1.5){
    let c = colorArgument(color)
    return c ? colorName(lighter16(c, percent(factor))) : color
}

function darkerColor(color, factor = 2.0){
    let c = colorArgument(color)
    return c ? colorName(darker16(c, percent(factor))) : color
}

// QColor::setAlphaF
function alphaColor(color, value){
    let c = colorArgument(color)
    if(!c) return color
    c.a = roundMul(F(Number(value)), USHRT_MAX)
    return colorName(c)
}

// QQuickColorProvider::tint: an opaque tint replaces, a transparent one leaves the base, else they are blended
function tintColor(base, tint){
    let b = colorArgument(base)
    if(!b) return base
    let t = colorArgument(tint)
    if(!t) return tint
    let tintAlpha = div257(t.a)
    if(tintAlpha === 0xff) return colorName(t)
    if(tintAlpha === 0) return colorName(b)
    const a = F(t.a / USHRT_MAX), inv = F(1 - a)
    const mix = (x, y) => F(F(F(x / USHRT_MAX) * a) + F(F(y / USHRT_MAX) * inv))
    return rgbaColor(mix(t.r, b.r), mix(t.g, b.g), mix(t.b, b.b), F(a + F(inv * F(b.a / USHRT_MAX))))
}

// QColor::toHsl of an Rgb color: { a, h (hundredths of a degree, USHRT_MAX achromatic), s, l }
function rgbToHsl(c){
    const r = F(c.r / USHRT_MAX), g = F(c.g / USHRT_MAX), b = F(c.b / USHRT_MAX)
    const max = Math.max(r, g, b), min = Math.min(r, g, b)
    const sum = F(max + min)
    const l = F(sum * 0.5)
    let hsl = { a: c.a, h: USHRT_MAX, s: 0, l: roundMul(l, USHRT_MAX) }
    if(max === min) return hsl
    const delta = F(max - min)
    const s = l < 0.5 ? F(delta / sum) : F(delta / F(2 - sum))
    let hue
    if(max === r) hue = F(F(g - b) / delta)
    else if(max === g) hue = F(2 + F(F(b - r) / delta))
    else hue = F(4 + F(F(r - g) / delta))
    hue = F(hue * 60)
    if(hue < 0) hue = F(hue + 360)
    hsl.h = roundMul(hue, 100)
    hsl.s = roundMul(s, USHRT_MAX)
    return hsl
}

// The components of a color value (a color is kept as its name, #rrggbb or #aarrggbb): color.r, .hsvHue, .valid ...
// as the QColor value type of QML gives them (QColor::redF, hsvHueF: floats; -1 is the hue of a gray)
const COLOR_TEXT = /^#(?:[0-9a-fA-F]{6}|[0-9a-fA-F]{8})$/
const hueF = (h) => (h === USHRT_MAX ? -1 : F(h / 36000))
const COMPONENTS = {
    r: (c) => F(c.r / USHRT_MAX), g: (c) => F(c.g / USHRT_MAX), b: (c) => F(c.b / USHRT_MAX), a: (c) => F(c.a / USHRT_MAX),
    hsvHue: (c) => hueF(rgbToHsv(c).h), hsvSaturation: (c) => F(rgbToHsv(c).s / USHRT_MAX), hsvValue: (c) => F(rgbToHsv(c).v / USHRT_MAX),
    hslHue: (c) => hueF(rgbToHsl(c).h), hslSaturation: (c) => F(rgbToHsl(c).s / USHRT_MAX), hslLightness: (c) => F(rgbToHsl(c).l / USHRT_MAX),
    valid: () => true,
}
function installComponents(){
    for(let name of Object.keys(COMPONENTS)){
        if(Object.prototype.hasOwnProperty.call(String.prototype, name)) continue
        let read = COMPONENTS[name]
        Object.defineProperty(String.prototype, name, {
            get(){
                let text = String(this)
                return COLOR_TEXT.test(text) ? read(parseColor(text)) : undefined
            },
            configurable: true,
        })
    }
}

// a color with one component set, or null when text is not a color: QQuickColorValueType::setR (QColor::setRedF), ...,
// setHsvHue (QColor::getHsvF, then setHsvF with the new hue), ...
function setComponent(text, name, value){
    if(typeof text !== 'string' || !COLOR_TEXT.test(text) || !COMPONENTS[name] || name === 'valid') return null
    let c = parseColor(text)
    let get = (n) => COMPONENTS[n](c)
    let v = Number(value)
    switch(name){
        case 'r': return rgbaColor(v, get('g'), get('b'), get('a'))
        case 'g': return rgbaColor(get('r'), v, get('b'), get('a'))
        case 'b': return rgbaColor(get('r'), get('g'), v, get('a'))
        case 'a': return rgbaColor(get('r'), get('g'), get('b'), v)
        case 'hsvHue': return hsvaColor(v, get('hsvSaturation'), get('hsvValue'), get('a'))
        case 'hsvSaturation': return hsvaColor(get('hsvHue'), v, get('hsvValue'), get('a'))
        case 'hsvValue': return hsvaColor(get('hsvHue'), get('hsvSaturation'), v, get('a'))
        case 'hslHue': return hslaColor(v, get('hslSaturation'), get('hslLightness'), get('a'))
        case 'hslSaturation': return hslaColor(get('hslHue'), v, get('hslLightness'), get('a'))
        case 'hslLightness': return hslaColor(get('hslHue'), get('hslSaturation'), v, get('a'))
    }
    return null
}

module.exports = {
    installComponents, rgbToHsl, setComponent,
    coerceColor, toCss, rgba, NAMED, parseColor, colorName, rgbaColor, hslaColor, hsvaColor, lighterColor, darkerColor,
    alphaColor, tintColor,
}
