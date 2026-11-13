const engine = require('../Core/engine')
const { toCss } = require('../Core/color')
const { defineType, makeReadonly } = require('../Core/type')
const { Font } = require('./Font')
const { Item, bits } = require('./Item')
const { NOWRAP, measure, style, wrapParagraph } = require('./TextLayout')

const STYLE = bits.STYLE

const TEXT = 64

const relayoutText = (text) => text.$updateImplicitSize()

const DEFAULT_FONT = { pixelSize: 12, family: 'Segoe UI', weight: 400, italic: false, underline: false, strikeout: false, capitalization: 0, letterSpacing: 0, wordSpacing: 0 }

function capitalize(text, mode){
    if(mode === 1) return text.toUpperCase()
    if(mode === 2) return text.toLowerCase()
    if(mode === 4) return text.replace(/(^|\s)(\S)/g, (m, s, c) => s + c.toUpperCase())
    return text
}

const ELIDE_LEFT = 0, ELIDE_RIGHT = 1, ELIDE_MIDDLE = 2, ELIDE_NONE = 3
const ELLIPSIS = '…'

// the longest form of line with an ellipsis that fits width
function elide(line, mode, width, font){
    if(measure(line, font).advance <= width) return line
    let fits = (s) => measure(s, font).advance <= width
    let lo = 0, hi = line.length
    let make = (n) => {
        if(mode === ELIDE_LEFT) return ELLIPSIS + line.slice(line.length - n)
        if(mode === ELIDE_MIDDLE){
            let head = Math.ceil(n / 2)
            return line.slice(0, head) + ELLIPSIS + line.slice(line.length - (n - head))
        }
        return line.slice(0, n) + ELLIPSIS
    }
    while(lo < hi){
        let mid = (lo + hi + 1) >> 1
        if(fits(make(mid))) lo = mid
        else hi = mid - 1
    }
    return lo > 0 || fits(ELLIPSIS) ? make(lo) : ''
}

// elements of QTextHtmlParser (qtexthtmlparser.cpp)
const HTML_ELEMENTS = new Set(['a', 'address', 'b', 'big', 'blockquote', 'body', 'br', 'caption', 'center', 'cite', 'code', 'dd', 'dfn',
    'div', 'dl', 'dt', 'em', 'font', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'head', 'hr', 'html', 'i', 'img', 'kbd', 'li', 'link', 'meta',
    'nobr', 'ol', 'p', 'pre', 'qt', 's', 'samp', 'script', 'small', 'span', 'strike', 'strong', 'style', 'sub', 'sup', 'table', 'tbody',
    'td', 'tfoot', 'th', 'thead', 'title', 'tr', 'tt', 'u', 'ul', 'var'])

// Qt::mightBeRichText (qtextdocument.cpp): the first tag of the first line must be an element Qt knows, so "<no name>" is plain
function mightBeRichText(text){
    let n = text.length, start = 0
    while(start < n && /\s/.test(text[start])) start++
    if(text.substr(start, 5) === '<?xml'){
        while(start < n){
            if(text[start] === '?' && start + 2 < n && text[start + 1] === '>'){ start += 2; break }
            start++
        }
        while(start < n && /\s/.test(text[start])) start++
    }
    if(text.substr(start, 5).toLowerCase() === '<!doc') return true
    let open = start
    while(open < n && text[open] !== '<' && text[open] !== '\n'){
        if(text[open] === '&' && text.substr(open + 1, 3) === 'lt;') return true
        open++
    }
    if(open >= n || text[open] !== '<') return false
    let close = text.indexOf('>', open)
    if(close < 0) return false
    let tag = ''
    for(let i = open + 1; i < close; i++){
        let c = text[i]
        let space = /\s/.test(c)
        if(/[\p{L}\p{Nd}]/u.test(c)) tag += c.toLowerCase()
        else if(tag && space) break
        else if(tag && c === '/' && i + 1 === close) break
        else if(!space && (tag || c !== '!')) return false
    }
    return HTML_ELEMENTS.has(tag)
}

const RICH_TAGS = new Set(['A', 'B', 'BIG', 'BLOCKQUOTE', 'BR', 'CENTER', 'CITE', 'CODE', 'DD', 'DFN', 'DIV', 'DL', 'DT', 'EM', 'FONT',
    'H1', 'H2', 'H3', 'H4', 'H5', 'H6', 'HR', 'I', 'IMG', 'KBD', 'LI', 'NOBR', 'OL', 'P', 'PRE', 'Q', 'S', 'SAMP', 'SMALL', 'SPAN',
    'STRIKE', 'STRONG', 'SUB', 'SUP', 'TABLE', 'TBODY', 'TD', 'TFOOT', 'TH', 'THEAD', 'TR', 'TT', 'U', 'UL', 'VAR'])
const RICH_ATTRIBUTES = new Set(['href', 'color', 'size', 'face', 'style', 'align', 'width', 'height', 'src', 'colspan', 'rowspan', 'alt', 'title'])

const URL_SCHEMES = new Set(['http', 'https', 'mailto', 'tel', 'ftp', 'qrc', 'file'])
// CSS properties of Qt's rich text subset (richtext-html-subset), none of them loads a url
const RICH_CSS = new Set(['color', 'background-color', 'font-family', 'font-size', 'font-style', 'font-weight', 'font-variant',
    'text-decoration', 'text-transform', 'text-indent', 'text-align', 'line-height', 'word-spacing', 'letter-spacing', 'white-space',
    'vertical-align', 'margin', 'margin-top', 'margin-bottom', 'margin-left', 'margin-right', 'padding', 'padding-top',
    'padding-bottom', 'padding-left', 'padding-right', 'border', 'border-width', 'border-style', 'border-color', 'border-collapse'])

// relative or of a known scheme, read the way a browser does: without tabs, line breaks and leading
// controls or spaces
function safeUrl(value, image){
    let url = value.replace(/[\t\n\r]/g, '').replace(/^[\u0000- ]+/, '')
    let scheme = /^([a-z][a-z0-9+.-]*):/i.exec(url)
    if(!scheme) return true
    let name = scheme[1].toLowerCase()
    return URL_SCHEMES.has(name) || (image && name === 'data' && /^data:image\/(png|gif|jpeg|webp|bmp)[;,]/i.test(url))
}

function safeStyle(value){
    return value.split(';').filter(rule => {
        let colon = rule.indexOf(':')
        return colon > 0 && RICH_CSS.has(rule.slice(0, colon).trim().toLowerCase()) && !/url\s*\(|expression\s*\(|\\/i.test(rule)
    }).join(';')
}

// the subset of HTML Qt renders, without scripts, handlers, unsafe urls or css that loads or covers the page
function sanitize(html){
    let template = document.createElement('template')
    template.innerHTML = html
    let walk = (node) => {
        for(let child of Array.from(node.childNodes)){
            if(child.nodeType === 1){
                if(!RICH_TAGS.has(child.tagName)){
                    if(child.tagName === 'SCRIPT' || child.tagName === 'STYLE') child.remove()
                    else {
                        walk(child)
                        child.replaceWith(...child.childNodes)
                    }
                    continue
                }
                for(let attr of Array.from(child.attributes)){
                    let name = attr.name.toLowerCase()
                    if(!RICH_ATTRIBUTES.has(name) || ((name === 'href' || name === 'src') && !safeUrl(attr.value, name === 'src'))) child.removeAttribute(attr.name)
                    else if(name === 'style') child.setAttribute(attr.name, safeStyle(attr.value))
                }
                if(child.tagName === 'A') child.style.pointerEvents = 'auto'
                walk(child)
            } else if(child.nodeType !== 3){
                child.remove()
            }
        }
    }
    walk(template.content)
    return template.content
}

class Text extends Item {
    constructor(parent = null){
        super(parent)
        this.$layout = null
        this.$text = null
        if(this.$dom){
            // the text sits in an inner block, aligned by the flex box like the current runtime renders it;
            // child items of the Text stay next to it
            this.$dom.className = 'jq2 jq2-text'
            this.$text = document.createElement('div')
            this.$text.className = 'jq2-text-inner'
            this.$dom.appendChild(this.$text)
            this.$dom.addEventListener('click', (e) => {
                let link = e.target && e.target.closest ? e.target.closest('a') : null
                if(!link) return
                e.preventDefault()
                engine.batch(() => engine.emitArgs(this, Text.SIGNAL_linkActivated, [link.getAttribute('href') || '']))
            })
        }
        this.$updateImplicitSize()
    }

    $font(){
        let font = this.$v[Text.FONT]
        return font ? font.$info() : DEFAULT_FONT
    }

    $isRich(){
        let format = this.$v[Text.TEXTFORMAT]
        if(format === Text.RichText || format === Text.StyledText) return true
        if(format === Text.AutoText) return mightBeRichText(this.$v[Text.TEXT])
        return false
    }

    $lineHeight(font){
        let natural = measure('M', font).height
        let lh = this.$v[Text.LINEHEIGHT]
        return this.$v[Text.LINEHEIGHTMODE] === 1 ? lh : natural * lh
    }

    $padding(side){
        let v = this.$v
        let own = v[side]
        return own === undefined || own < 0 || own !== own ? v[Text.PADDING] : own
    }

    // QQuickTextPrivate::updateSize: natural size for the implicit size, wrapped / elided lines for painting
    $relayout(){
        let v = this.$v
        let font = this.$font()
        let text = capitalize(String(v[Text.TEXT]), font.capitalization)
        let left = this.$padding(Text.LEFTPADDING), right = this.$padding(Text.RIGHTPADDING)
        let top = this.$padding(Text.TOPPADDING), bottom = this.$padding(Text.BOTTOMPADDING)
        let lh = this.$lineHeight(font)
        if(this.$isRich()){
            this.$layout = { rich: true, text, lh, left, right, top, bottom }
            return this.$layout
        }
        let paragraphs = text.replace(/\r\n?/g, '\n').replace(/\u2028/g, '\n').split('\n')
        let natural = 0
        for(let p of paragraphs) natural = Math.max(natural, p ? measure(p, font).width : 0)

        let explicitWidth = this.$explicit & 1
        let explicitHeight = this.$explicit & 2
        let available = explicitWidth ? Math.max(0, v[Item.WIDTH] - left - right) : Infinity
        let wrap = v[Text.WRAPMODE]
        let lines = []
        if(wrap !== NOWRAP && available !== Infinity) for(let p of paragraphs) wrapParagraph(p, wrap, available, font, lines)
        else lines = paragraphs.slice()

        let mode = v[Text.ELIDE]
        let maxLines = v[Text.MAXIMUMLINECOUNT]
        let limit = maxLines > 0 ? maxLines : Infinity
        if(mode !== ELIDE_NONE && wrap !== NOWRAP && explicitHeight && lh > 0){
            limit = Math.min(limit, Math.max(1, Math.floor((v[Item.HEIGHT] - top - bottom) / lh)))
        }
        let truncated = false
        if(lines.length > limit){
            truncated = true
            let rest = lines.slice(limit - 1).join(' ')
            lines = lines.slice(0, limit)
            if(mode === ELIDE_RIGHT && available !== Infinity) lines[limit - 1] = elide(rest, ELIDE_RIGHT, available, font)
        }
        if(mode !== ELIDE_NONE && available !== Infinity && (wrap === NOWRAP || mode !== ELIDE_RIGHT)){
            lines = lines.map(line => {
                let e = elide(line, mode, available, font)
                if(e !== line) truncated = true
                return e
            })
        }
        let contentWidth = 0
        for(let line of lines) contentWidth = Math.max(contentWidth, line ? measure(line, font).width : 0)
        this.$layout = { rich: false, lines, lh, left, right, top, bottom, natural, contentWidth, contentHeight: lines.length * lh, truncated }
        return this.$layout
    }

    $updateImplicitSize(){
        let layout = this.$relayout()
        let v = this.$v
        engine.markDirty(this, TEXT)
        if(layout.rich){
            this.$measureRich(layout)
        }
        let unwrappedLines = layout.rich ? 1 : String(v[Text.TEXT]).split('\n').length
        let maxLines = v[Text.MAXIMUMLINECOUNT]
        let heightLines = layout.rich ? 1 : (this.$explicit & 1) && v[Text.WRAPMODE] !== NOWRAP ? layout.lines.length : Math.min(unwrappedLines, maxLines > 0 ? maxLines : Infinity)
        let width = (layout.rich ? layout.naturalWidth : layout.natural) + layout.left + layout.right
        let textHeight = heightLines * layout.lh
        // several lines at the font's own line height are one block, rounded once like the current runtime measures it
        if(heightLines > 1 && v[Text.LINEHEIGHTMODE] === 0 && v[Text.LINEHEIGHT] === 1) textHeight = Math.ceil(heightLines * measure('M', this.$font()).line + 1)
        let height = (layout.rich ? layout.implicitHeight : textHeight) + layout.top + layout.bottom
        this.$write(Item.IMPLICITWIDTH, Math.ceil(width))
        this.$write(Item.IMPLICITHEIGHT, layout.rich ? height : Math.ceil(height))
        let contentWidth = layout.rich ? layout.naturalWidth : layout.contentWidth
        let contentHeight = layout.rich ? layout.naturalHeight : layout.contentHeight
        this.$write(Text.CONTENTWIDTH, contentWidth)
        this.$write(Text.CONTENTHEIGHT, contentHeight)
        this.$write(Text.PAINTEDWIDTH, contentWidth)
        this.$write(Text.PAINTEDHEIGHT, contentHeight)
        // QQuickTextPrivate::updateBaselineOffset: the first line's ascent below the top padding
        this.$write(Item.BASELINEOFFSET, layout.top + measure('M', this.$font()).ascent)
        this.$write(Text.LINECOUNT, layout.rich ? 1 : layout.lines.length)
        this.$write(Text.TRUNCATED, !!layout.truncated)
    }

    $measureRich(layout){
        if(typeof document === 'undefined'){
            layout.naturalWidth = layout.text.length * 7
            layout.naturalHeight = layout.implicitHeight = layout.lh
            return
        }
        let probe = document.createElement('div')
        probe.className = 'jq2-text-inner'
        probe.style.cssText = 'position:absolute;visibility:hidden;left:-10000px;top:0;white-space:' + (this.$v[Text.WRAPMODE] !== NOWRAP && (this.$explicit & 1) ? 'normal' : 'nowrap') + ';'
        this.$styleFont(probe.style, this.$font(), layout.lh)
        if((this.$explicit & 1) && this.$v[Text.WRAPMODE] !== NOWRAP) probe.style.width = Math.max(0, this.$v[Item.WIDTH] - layout.left - layout.right) + 'px'
        probe.appendChild(sanitize(layout.text))
        document.body.appendChild(probe)
        layout.naturalWidth = Math.ceil(probe.scrollWidth)
        layout.naturalHeight = layout.implicitHeight = Math.ceil(probe.scrollHeight)
        // a QTextDocument with a line height: each line is the height in QFixed (1/64), and the last one does not count its extra space
        if(this.$v[Text.LINEHEIGHTMODE] !== 0 || this.$v[Text.LINEHEIGHT] !== 1){
            let span = Math.floor(layout.lh * 64) / 64
            layout.naturalHeight = Math.max(1, Math.round(probe.scrollHeight / layout.lh)) * span
            layout.implicitHeight = layout.naturalHeight - Math.floor(span - measure('M', this.$font()).height)
        }
        probe.remove()
    }

    $styleFont(s, font, lh){
        s.fontSize = font.pixelSize + 'px'
        s.fontFamily = font.family
        s.fontWeight = font.weight
        s.fontStyle = font.italic ? 'italic' : ''
        s.textDecoration = [font.underline ? 'underline' : '', font.strikeout ? 'line-through' : ''].join(' ').trim()
        s.fontVariant = font.capitalization === 3 ? 'small-caps' : ''
        s.letterSpacing = font.letterSpacing ? font.letterSpacing + 'px' : ''
        s.wordSpacing = font.wordSpacing ? font.wordSpacing + 'px' : ''
        // the default line height is the font's own, as the current runtime renders it
        let natural = this.$v[Text.LINEHEIGHTMODE] === 0 && this.$v[Text.LINEHEIGHT] === 1
        s.lineHeight = natural ? 'normal' : lh + 'px'
    }

    $write(id, value){
        super.$write(id, value)
        if((id === Item.WIDTH && (this.$explicit & 1)) || (id === Item.HEIGHT && (this.$explicit & 2))){
            if(this.$v[Text.WRAPMODE] !== NOWRAP || this.$v[Text.ELIDE] !== ELIDE_NONE || this.$isRich()) this.$updateImplicitSize()
            else engine.markDirty(this, TEXT)
        }
    }

    $paint(dirty){
        super.$paint(dirty)
        if(!this.$dom) return
        let s = this.$dom.style
        if(dirty & STYLE) s.color = toCss(this.$v[Text.COLOR])
        if(dirty & (TEXT | bits.GEOMETRY)){
            let layout = this.$layout || this.$relayout()
            let v = this.$v
            if(dirty & TEXT){
                this.$styleFont(s, this.$font(), layout.lh)
                let h = v[Text.HORIZONTALALIGNMENT]
                s.textAlign = h & 2 ? 'right' : h & 4 ? 'center' : h & 8 ? 'justify' : 'left'
                s.justifyContent = h & 2 ? 'flex-end' : h & 4 ? 'center' : 'flex-start'
                let valign = v[Text.VERTICALALIGNMENT]
                s.alignItems = valign & 0x40 ? 'flex-end' : valign & 0x80 ? 'center' : 'flex-start'
                s.paddingLeft = layout.left + 'px'
                s.paddingRight = layout.right + 'px'
                s.paddingTop = layout.top + 'px'
                s.paddingBottom = layout.bottom + 'px'
                let inner = this.$text
                if(layout.rich){
                    inner.style.whiteSpace = v[Text.WRAPMODE] !== NOWRAP && (this.$explicit & 1) ? 'normal' : 'nowrap'
                    inner.replaceChildren(sanitize(layout.text))
                } else {
                    inner.style.whiteSpace = 'pre'
                    inner.textContent = layout.lines.join('\n')
                }
            }
        }
    }

    linkAt(x, y){
        if(!this.$dom || typeof document === 'undefined') return ''
        let rect = this.$dom.getBoundingClientRect()
        let el = document.elementFromPoint(rect.left + x, rect.top + y)
        let link = el && el.closest ? el.closest('a') : null
        return link && this.$dom.contains(link) ? link.getAttribute('href') || '' : ''
    }

    forceLayout(){ this.$updateImplicitSize() }
}

const paddingChanged = (text) => text.$updateImplicitSize()

defineType(Text, {
    props: {
        text: { kind: 'string', effect: relayoutText },
        color: { kind: 'color', effect: style },
        font: { group: Font },
        horizontalAlignment: { kind: 'int', initial: 1, effect: (t) => engine.markDirty(t, TEXT) },
        verticalAlignment: { kind: 'int', initial: 32, effect: (t) => engine.markDirty(t, TEXT) },
        wrapMode: { kind: 'int', effect: relayoutText },
        elide: { kind: 'int', initial: ELIDE_NONE, effect: relayoutText },
        maximumLineCount: { kind: 'int', initial: 2147483647, effect: relayoutText },
        lineHeight: { kind: 'real', initial: 1, effect: relayoutText },
        lineHeightMode: { kind: 'int', effect: relayoutText },
        textFormat: { kind: 'int', initial: 2, effect: relayoutText },
        padding: { kind: 'real', effect: paddingChanged },
        leftPadding: { kind: 'real', initial: NaN, effect: paddingChanged },
        rightPadding: { kind: 'real', initial: NaN, effect: paddingChanged },
        topPadding: { kind: 'real', initial: NaN, effect: paddingChanged },
        bottomPadding: { kind: 'real', initial: NaN, effect: paddingChanged },
        lineCount: { kind: 'int', initial: 1, readonly: true },
        contentWidth: { kind: 'real', readonly: true },
        contentHeight: { kind: 'real', readonly: true },
        paintedWidth: { kind: 'real', readonly: true },
        paintedHeight: { kind: 'real', readonly: true },
        truncated: { kind: 'bool', readonly: true },
        hoveredLink: { kind: 'string', readonly: true },
        linkColor: { kind: 'color', initial: '#0000ff' },
        style: 'int',
        styleColor: 'color',
        baseUrl: 'string',
        renderType: 'int',
        fontSizeMode: 'int',
        minimumPixelSize: { kind: 'int', initial: 12 },
        minimumPointSize: { kind: 'int', initial: 12 },
        antialiasing: { kind: 'bool', initial: true },
    },
    signals: ['linkActivated(link)', 'linkHovered(link)', 'lineLaidOut(line)'],
})

Object.assign(Text, { AlignLeft: 0x1, AlignRight: 0x2, AlignHCenter: 0x4, AlignJustify: 0x8, AlignTop: 0x20, AlignBottom: 0x40, AlignVCenter: 0x80 }, {
    NoWrap: 0, WordWrap: 1, WrapAnywhere: 3, Wrap: 4, WrapAtWordBoundaryOrAnywhere: 4,
    ElideLeft: 0, ElideRight: 1, ElideMiddle: 2, ElideNone: 3,
    PlainText: 0, RichText: 1, AutoText: 2, StyledText: 4, MarkdownText: 3,
    ProportionalHeight: 0, FixedHeight: 1,
    Normal: 0, Outline: 1, Raised: 2, Sunken: 3,
    FixedSize: 0, HorizontalFit: 1, VerticalFit: 2, Fit: 3,
    QtRendering: 0, NativeRendering: 1, CurveRendering: 2,
})

// read-only in Qt, writable in the base class
makeReadonly(Text, 'implicitWidth')
makeReadonly(Text, 'implicitHeight')

module.exports = { Text }
