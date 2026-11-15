const engine = require('../Core/engine')
const { bits } = require('./Item')

const STYLE = bits.STYLE

const style = (item) => engine.markDirty(item, STYLE)

// ---- Text ---------------------------------------------------------------------

let canvasContext = null
const measureCache = new Map()

function fontCss(f){
    return (f.italic ? 'italic ' : '') + (f.weight !== 400 ? f.weight + ' ' : '') + f.pixelSize + 'px ' + f.family
}

// Same metrics as core/TextController.measureTextFast of the current runtime
function measure(text, pixelSize, family, bold, italic, letterSpacing){
    let f = typeof pixelSize === 'object' ? pixelSize : { pixelSize, family, weight: bold ? 700 : 400, italic: !!italic, letterSpacing: letterSpacing || 0 }
    let font = fontCss(f)
    let key = font + '\n' + text
    let size = measureCache.get(key)
    if(size) return size
    if(typeof document === 'undefined') return { width: text.length * f.pixelSize * 0.5, advance: text.length * f.pixelSize * 0.5, height: Math.ceil(f.pixelSize * 1.33), line: f.pixelSize * 1.33, ascent: f.pixelSize * 1.06 }
    if(!canvasContext) canvasContext = document.createElement('canvas').getContext('2d')
    canvasContext.font = font
    let metrics = canvasContext.measureText(text)
    // advance is the pen advance (QFontMetricsF::horizontalAdvance) that elide and wrap compare with the width;
    // width also covers glyphs reaching past it, like the current runtime's implicit sizes
    let advance = metrics.width + (f.letterSpacing ? f.letterSpacing * text.length : 0)
    let width = metrics.width
    if(Number.isFinite(metrics.actualBoundingBoxLeft) && Number.isFinite(metrics.actualBoundingBoxRight)){
        width = Math.max(width, metrics.actualBoundingBoxLeft + metrics.actualBoundingBoxRight)
    }
    if(f.letterSpacing) width += f.letterSpacing * text.length
    let height = Number.isFinite(metrics.fontBoundingBoxAscent)
        ? metrics.fontBoundingBoxAscent + metrics.fontBoundingBoxDescent
        : Math.ceil(f.pixelSize * 1.2)
    let ascent = Number.isFinite(metrics.fontBoundingBoxAscent) ? metrics.fontBoundingBoxAscent : f.pixelSize
    size = { width: Math.ceil(width + 1), advance, height: Math.ceil(height), line: height, ascent }
    if(measureCache.size > 10000) measureCache.clear()
    measureCache.set(key, size)
    return size
}

const NOWRAP = 0, WORDWRAP = 1, WRAPANYWHERE = 3, WRAP = 4

// greedy line breaking of one paragraph
function wrapParagraph(paragraph, mode, width, font, out){
    if(paragraph === ''){
        out.push('')
        return
    }
    if(mode === WRAPANYWHERE){
        let line = ''
        for(let ch of paragraph){
            if(line && measure(line + ch, font).advance > width){
                out.push(line)
                line = ch
            } else line += ch
        }
        out.push(line)
        return
    }
    let tokens = paragraph.split(/(\s+)/).filter(t => t !== '')
    let line = ''
    for(let token of tokens){
        let candidate = line + token
        if(!line || measure(candidate.replace(/\s+$/, ''), font).advance <= width){
            line = candidate
            continue
        }
        if(/^\s+$/.test(token)){
            line = candidate
            continue
        }
        out.push(line.replace(/\s+$/, ''))
        line = token
        // a word longer than the line: WordWrap lets it overflow, Wrap breaks it
        while(mode === WRAP && measure(line, font).advance > width && line.length > 1){
            let n = line.length - 1
            while(n > 1 && measure(line.slice(0, n), font).advance > width) n--
            out.push(line.slice(0, n))
            line = line.slice(n)
        }
    }
    out.push(line.replace(/\s+$/, ''))
}

module.exports = { style, fontCss, measure, NOWRAP, wrapParagraph }
