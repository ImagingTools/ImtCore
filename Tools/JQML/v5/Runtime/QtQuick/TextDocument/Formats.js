// The formats of a text document (QTextCharFormat, QTextBlockFormat, QTextListFormat, QTextImageFormat of Qt 6.12): plain
// objects of the properties that are set, compared by value. A format is not changed once it is made; the functions here
// return new ones.

// QTextCharFormat::VerticalAlignment
const ALIGN_NORMAL = 0, ALIGN_SUPERSCRIPT = 1, ALIGN_SUBSCRIPT = 2, ALIGN_MIDDLE = 3, ALIGN_TOP = 4, ALIGN_BOTTOM = 5

// QTextCharFormat::UnderlineStyle
const NO_UNDERLINE = 0, SINGLE_UNDERLINE = 1

// QTextListFormat::Style
const LIST_DISC = -1, LIST_CIRCLE = -2, LIST_SQUARE = -3, LIST_DECIMAL = -4, LIST_LOWER_ALPHA = -5, LIST_UPPER_ALPHA = -6,
    LIST_LOWER_ROMAN = -7, LIST_UPPER_ROMAN = -8, LIST_UNDEFINED = 0

// Qt::Alignment of a block
const ALIGN_LEFT = 0x1, ALIGN_RIGHT = 0x2, ALIGN_HCENTER = 0x4, ALIGN_JUSTIFY = 0x8, ALIGN_ABSOLUTE = 0x10, ALIGN_CENTER = 0x84

const EMPTY = Object.freeze({})

function same(a, b){
    if(a === b) return true
    if(Array.isArray(a) && Array.isArray(b)) return a.length === b.length && a.every((v, i) => same(v, b[i]))
    if(a && b && typeof a === 'object' && typeof b === 'object' && !a.$identity && !b.$identity){
        let ka = Object.keys(a), kb = Object.keys(b)
        return ka.length === kb.length && ka.every(k => same(a[k], b[k]))
    }
    return false
}

// two formats with the same properties
function equal(a, b){
    if(a === b) return true
    let ka = Object.keys(a), kb = Object.keys(b)
    if(ka.length !== kb.length) return false
    for(let k of ka) if(!(k in b) || !same(a[k], b[k])) return false
    return true
}

function with_(format, changes){
    let next = Object.assign({}, format)
    for(let k in changes){
        if(changes[k] === undefined) delete next[k]
        else next[k] = changes[k]
    }
    return Object.freeze(next)
}

function without(format, ...keys){
    let next = Object.assign({}, format)
    for(let k of keys) delete next[k]
    return Object.freeze(next)
}

// QTextFormat::merge: the properties of other over those of format
function merge(format, other){
    return Object.freeze(Object.assign({}, format, other))
}

function count(format){
    return Object.keys(format).length
}

// ---- char format reads with the defaults of QTextCharFormat ----

const fontWeight = (f) => 'weight' in f ? f.weight : 400
const fontItalic = (f) => !!f.italic
// QTextCharFormat::fontUnderline: the underline style when it is set
const fontUnderline = (f) => 'underlineStyle' in f ? f.underlineStyle === SINGLE_UNDERLINE : !!f.fontUnderline
const fontOverline = (f) => !!f.overline
const fontStrikeOut = (f) => !!f.strikeOut
const verticalAlignment = (f) => f.verticalAlignment || ALIGN_NORMAL
const fontCapitalization = (f) => f.capitalization || 0
const fontWordSpacing = (f) => f.wordSpacing || 0

// QTextCharFormat::setFont(font, FontPropertiesSpecifiedOnly): the properties the font has set
function setFont(format, font){
    let changes = {}
    if(font.families) changes.families = font.families.slice()
    if(font.pointSize > 0) changes.pointSize = font.pointSize
    else if(font.pixelSize > 0) changes.pixelSize = font.pixelSize
    if('weight' in font) changes.weight = font.weight
    if('italic' in font) changes.italic = font.italic
    if('underline' in font){
        changes.underlineStyle = font.underline ? SINGLE_UNDERLINE : NO_UNDERLINE
        changes.fontUnderline = !!font.underline
    }
    if('overline' in font) changes.overline = font.overline
    if('strikeOut' in font) changes.strikeOut = font.strikeOut
    if('capitalization' in font) changes.capitalization = font.capitalization
    if('wordSpacing' in font) changes.wordSpacing = font.wordSpacing
    if(font.letterSpacing){
        changes.letterSpacingType = font.letterSpacing.type
        changes.letterSpacing = font.letterSpacing.value
    }
    if('kerning' in font) changes.kerning = font.kerning
    return with_(format, changes)
}

// the char format of the default font of a document (QTextHtmlExporter: defaultCharFormat.setFont(doc->defaultFont()))
function fontFormat(font){
    let f = {
        families: [font.family],
        weight: font.weight,
        italic: !!font.italic,
        underlineStyle: font.underline ? SINGLE_UNDERLINE : NO_UNDERLINE,
        fontUnderline: !!font.underline,
        overline: false,
        strikeOut: !!font.strikeout,
        capitalization: font.capitalization || 0,
        wordSpacing: font.wordSpacing || 0,
        // QFont: percentage spacing of 0 unless an absolute spacing was set (QQuickFontValueType::setLetterSpacing)
        letterSpacingType: font.letterSpacing ? 'absolute' : 'percent',
        letterSpacing: font.letterSpacing || 0,
        kerning: true,
    }
    if(font.sizeUnit === 'px') f.pixelSize = font.pixelSize
    else f.pointSize = font.pointSize
    return Object.freeze(f)
}

module.exports = {
    EMPTY, equal, with_, without, merge, count, same,
    fontWeight, fontItalic, fontUnderline, fontOverline, fontStrikeOut, verticalAlignment, fontCapitalization, fontWordSpacing,
    setFont, fontFormat,
    ALIGN_NORMAL, ALIGN_SUPERSCRIPT, ALIGN_SUBSCRIPT, ALIGN_MIDDLE, ALIGN_TOP, ALIGN_BOTTOM,
    NO_UNDERLINE, SINGLE_UNDERLINE,
    LIST_DISC, LIST_CIRCLE, LIST_SQUARE, LIST_DECIMAL, LIST_LOWER_ALPHA, LIST_UPPER_ALPHA, LIST_LOWER_ROMAN, LIST_UPPER_ROMAN, LIST_UNDEFINED,
    ALIGN_LEFT, ALIGN_RIGHT, ALIGN_HCENTER, ALIGN_JUSTIFY, ALIGN_ABSOLUTE, ALIGN_CENTER,
}
