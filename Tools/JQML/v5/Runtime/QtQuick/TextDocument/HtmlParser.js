// QTextHtmlParser (qtexthtmlparser.cpp of Qt 6.12): the HTML of a rich text as a list of nodes, each with its parent, its
// text (the text that follows its tag up to the next tag), its char and block format and the properties the importer
// reads. Node 0 is the root.
const Css = require('./Css')
const F = require('./Formats')
const { ENTITIES } = require('./Entities')

// display modes (QTextHtmlElement::DisplayMode)
const BLOCK = 'block', INLINE = 'inline', TABLE = 'table', NONE = 'none'

// the elements (elements[]): the name gives the id, "qt" is the body
const ELEMENTS = {
    a: INLINE, address: INLINE, b: INLINE, big: INLINE, blockquote: BLOCK, body: BLOCK, br: INLINE, caption: BLOCK,
    center: BLOCK, cite: INLINE, code: INLINE, dd: BLOCK, del: INLINE, dfn: INLINE, div: BLOCK, dl: BLOCK, dt: BLOCK,
    em: INLINE, font: INLINE, h1: BLOCK, h2: BLOCK, h3: BLOCK, h4: BLOCK, h5: BLOCK, h6: BLOCK, head: NONE, hr: BLOCK,
    html: INLINE, i: INLINE, img: INLINE, kbd: INLINE, li: BLOCK, link: NONE, meta: NONE, nobr: INLINE, ol: BLOCK,
    p: BLOCK, pre: BLOCK, qt: BLOCK, s: INLINE, samp: INLINE, script: NONE, small: INLINE, span: INLINE, strong: INLINE,
    style: NONE, sub: INLINE, sup: INLINE, table: TABLE, tbody: TABLE, td: BLOCK, tfoot: TABLE, th: BLOCK, thead: TABLE,
    title: NONE, tr: TABLE, tt: INLINE, u: INLINE, ul: BLOCK, var: INLINE,
}

// the fixed font of the platform (QFontDatabase::systemFont(FixedFont))
const FIXED_FAMILY = 'monospace'

// white space modes (QTextHtmlParserNode::WhiteSpaceMode)
const WS_NORMAL = 'normal', WS_PRE = 'pre', WS_NOWRAP = 'nowrap', WS_PREWRAP = 'pre-wrap', WS_PRELINE = 'pre-line', WS_UNDEFINED = undefined

const MARGIN_TOP = 0, MARGIN_RIGHT = 1, MARGIN_BOTTOM = 2, MARGIN_LEFT = 3

// QChar::isSpace
const isSpace = (c) => c !== undefined && /[\t\n\v\f\r \u0085\u00a0\u1680\u2000-\u200a\u2028\u2029\u202f\u205f\u3000]/.test(c)

// windowsLatin1ExtendedCharacters: &#128; .. &#159; as Windows-1252 has them
const WINDOWS_LATIN1 = [0x20ac, 0x0081, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021, 0x02c6, 0x2030, 0x0160, 0x2039, 0x0152,
    0x008d, 0x017d, 0x008f, 0x0090, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 0x02dc, 0x2122, 0x0161, 0x203a,
    0x0153, 0x009d, 0x017e, 0x0178]

const BORDER_STYLE_NONE = 'none', BORDER_STYLE_OUTSET = 'outset'

class Node {
    constructor(){
        this.tag = ''
        this.text = ''
        this.attributes = []
        this.parent = 0
        this.children = []
        this.id = null
        this.charFormat = F.EMPTY
        this.blockFormat = F.EMPTY
        this.cssFloat = 'inflow'
        this.hasOwnListStyle = false
        this.hasOwnLineHeightType = false
        this.hasLineHeightMultiplier = false
        this.hasCssListIndent = false
        this.isEmptyParagraph = false
        this.isTextFrame = false
        this.isRootFrame = false
        this.displayMode = INLINE
        this.hasHref = false
        this.listStyle = F.LIST_UNDEFINED
        this.listStart = 1
        this.textListNumberPrefix = null
        this.textListNumberSuffix = null
        this.imageName = ''
        this.imageAlt = ''
        this.imageWidth = -1
        this.imageHeight = -1
        this.width = null
        this.height = null
        this.tableBorder = 0
        this.tableCellRowSpan = 1
        this.tableCellColSpan = 1
        this.tableCellSpacing = 2
        this.tableCellPadding = 0
        this.tableCellBorder = [0, 0, 0, 0]
        this.tableCellBorderBrush = [null, null, null, null]
        this.tableCellBorderStyle = [BORDER_STYLE_NONE, BORDER_STYLE_NONE, BORDER_STYLE_NONE, BORDER_STYLE_NONE]
        this.borderBrush = '#808080'
        this.borderStyle = BORDER_STYLE_OUTSET
        this.borderCollapse = false
        this.userState = -1
        this.cssListIndent = 0
        this.wsm = WS_UNDEFINED
        this.margin = [0, 0, 0, 0]
        this.padding = [-1, -1, -1, -1]
    }

    isListStart(){ return this.id === 'ol' || this.id === 'ul' }
    isTableCell(){ return this.id === 'td' || this.id === 'th' }
    isBlock(){ return this.displayMode === BLOCK }
    isNotSelfNesting(){ return this.id === 'p' || this.id === 'li' }
    mayNotHaveChildren(){ return this.id === 'img' || this.id === 'hr' || this.id === 'br' || this.id === 'meta' }

    allowedInContext(parentId){
        switch(this.id){
            case 'dd': case 'dt': return parentId === 'dl'
            case 'tr': return parentId === 'table' || parentId === 'thead' || parentId === 'tbody' || parentId === 'tfoot'
            case 'th': case 'td': return parentId === 'tr'
            case 'thead': case 'tbody': case 'tfoot': return parentId === 'table'
            case 'caption': return parentId === 'table'
            case 'body': return parentId !== 'head'
        }
        return true
    }

    hasOnlyWhitespace(){
        for(let c of this.text) if(!isSpace(c) || c === '\u2028') return false
        return true
    }

    isNestedList(parser){
        if(!this.isListStart()) return false
        for(let p = this.parent; p; p = parser.at(p).parent) if(parser.at(p).isListStart()) return true
        return false
    }

    // QTextHtmlParserNode::initializeProperties: what the node inherits of its parent and what its element sets
    initializeProperties(parent, parser){
        let cf = Object.assign({}, parent.charFormat)
        let bf = {}
        if(this.id === 'html') bf.layoutDirection = 0
        else if('layoutDirection' in parent.blockFormat) bf.layoutDirection = parent.blockFormat.layoutDirection
        if(parent.displayMode === NONE) this.displayMode = NONE
        if(parent.id !== 'table' || this.id === 'caption'){
            if('alignment' in parent.blockFormat) bf.alignment = parent.blockFormat.alignment
        }
        // the background is inherited only between inline elements and into non-body blocks (merged blocks)
        if((parent.id !== 'tr' || !this.isTableCell())
            && (this.displayMode !== INLINE || parent.displayMode !== INLINE)
            && (parent.id === 'body' || this.displayMode !== BLOCK || parent.displayMode !== BLOCK)){
            delete cf.background
        }
        this.listStyle = parent.listStyle
        delete cf.anchorNames
        this.wsm = parent.wsm
        this.margin = [0, 0, 0, 0]
        this.cssFloat = 'inflow'
        this.padding = [-1, -1, -1, -1]
        switch(this.id){
            case 'a':
                for(let i = 0; i < this.attributes.length; i += 2){
                    if(this.attributes[i].toLowerCase() === 'href' && this.attributes[i + 1] !== '') this.hasHref = true
                }
                cf.isAnchor = true
                break
            case 'big': cf.sizeAdjustment = 1; break
            case 'small': cf.sizeAdjustment = -1; break
            case 'h1': cf.sizeAdjustment = 3; this.margin[MARGIN_TOP] = 18; this.margin[MARGIN_BOTTOM] = 12; break
            case 'h2': cf.sizeAdjustment = 2; this.margin[MARGIN_TOP] = 16; this.margin[MARGIN_BOTTOM] = 12; break
            case 'h3': cf.sizeAdjustment = 1; this.margin[MARGIN_TOP] = 14; this.margin[MARGIN_BOTTOM] = 12; break
            case 'h4': cf.sizeAdjustment = 0; this.margin[MARGIN_TOP] = 12; this.margin[MARGIN_BOTTOM] = 12; break
            case 'h5': cf.sizeAdjustment = -1; this.margin[MARGIN_TOP] = 12; this.margin[MARGIN_BOTTOM] = 4; break
            case 'p': this.margin[MARGIN_TOP] = 12; this.margin[MARGIN_BOTTOM] = 12; break
            case 'ul': case 'ol':
                // nested lists have no margins, only the top level one
                if(!this.isNestedList(parser)){
                    this.margin[MARGIN_TOP] = 12
                    this.margin[MARGIN_BOTTOM] = 12
                }
                break
            case 'br': this.text = '\u2028'; break
            case 'pre': this.margin[MARGIN_TOP] = 12; this.margin[MARGIN_BOTTOM] = 12; break
            case 'blockquote':
                this.margin = [12, 40, 12, 40]
                bf.blockQuoteLevel = 1
                break
            case 'dl': this.margin[MARGIN_TOP] = 8; this.margin[MARGIN_BOTTOM] = 8; break
            case 'dd': this.margin[MARGIN_LEFT] = 30; break
        }
        this.charFormat = Object.freeze(cf)
        this.blockFormat = Object.freeze(bf)
    }

    setListStyle(values){
        let map = { 'none': F.LIST_UNDEFINED, 'disc': F.LIST_DISC, 'square': F.LIST_SQUARE, 'circle': F.LIST_CIRCLE,
            'decimal': F.LIST_DECIMAL, 'lower-alpha': F.LIST_LOWER_ALPHA, 'upper-alpha': F.LIST_UPPER_ALPHA,
            'lower-roman': F.LIST_LOWER_ROMAN, 'upper-roman': F.LIST_UPPER_ROMAN }
        for(let v of values){
            if(v.type === 'KnownIdentifier' && v.variant in map){
                this.hasOwnListStyle = true
                this.listStyle = map[v.variant]
            }
        }
        // a list item may have its own style
        if(this.id === 'li' && this.hasOwnListStyle) this.blockFormat = F.with_(this.blockFormat, { listStyle: this.listStyle })
    }

    // QTextHtmlParserNode::applyCssDeclarations
    applyCssDeclarations(declarations){
        let cf = Object.assign({}, this.charFormat)
        let bf = Object.assign({}, this.blockFormat)
        Css.extractBox(declarations, this.margin, this.padding)
        let borders = (widths, brushes, styles) => {
            let w = [0, 0, 0, 0], c = [null, null, null, null], s = [null, null, null, null]
            let hit = Css.extractBorder(declarations, w, c, s)
            for(let i = 0; i < 4; i++){
                widths[i] = w[i]
                if(brushes) brushes[i] = c[i]
                styles[i] = s[i] && s[i] !== 'native' ? s[i] : BORDER_STYLE_NONE
            }
            return { hit, c }
        }
        if(this.id === 'td' || this.id === 'th') borders(this.tableCellBorder, this.tableCellBorderBrush, this.tableCellBorderStyle)
        for(let decl of declarations){
            if(!decl.values.length) continue
            let first = decl.values[0]
            let identifier = first.type === 'KnownIdentifier' ? first.variant : null
            switch(decl.property){
                case 'border-color': {
                    let color = Css.colorOf(first)
                    if(color && color !== 'invalid') this.borderBrush = color
                    break
                }
                case 'border-style': {
                    let style = Css.borderStyleOf(first)
                    if(style && style !== 'native') this.borderStyle = style
                    break
                }
                case 'border-width': this.tableBorder = Css.lengthValues(decl, null)[0]; break
                case 'border': {
                    let w = [0, 0, 0, 0], s = [0, 0, 0, 0]
                    let { hit, c } = borders(w, null, s)
                    if(hit){
                        this.tableBorder = w[0]
                        if(c[0]) this.borderBrush = c[0]
                        this.borderStyle = s[0]
                    }
                    break
                }
                case 'border-collapse': this.borderCollapse = identifier === null && String(first.variant).toLowerCase() === 'collapse'; break
                case 'color': {
                    let color = Css.declarationColor(decl)
                    if(color === 'invalid') cf.foreground = '#000000'
                    else if(color) cf.foreground = color
                    break
                }
                case 'float': this.cssFloat = identifier === 'left' ? 'left' : identifier === 'right' ? 'right' : 'inflow'; break
                case '-qt-block-indent': bf.indent = Math.trunc(Number(first.variant)) || 0; break
                case '-qt-line-height-type': {
                    let name = String(first.variant).toLowerCase()
                    let type = name === 'proportional' ? 1 : name === 'fixed' ? 2 : name === 'minimum' ? 3 : name === 'line-distance' ? 4 : 0
                    if(this.hasLineHeightMultiplier) bf.lineHeight = (bf.lineHeight || 0) / 100
                    bf.lineHeightType = type
                    this.hasOwnLineHeightType = true
                    break
                }
                case 'line-height': {
                    let lineHeight, type
                    let px = Css.realValue(decl, 'px')
                    if(px !== null){
                        lineHeight = px
                        type = 3
                    } else {
                        let n = Number(first.variant)
                        if(!Number.isNaN(n) && String(first.variant).trim() !== ''){
                            lineHeight = n
                            if(!this.hasOwnLineHeightType && first.type === 'Number'){
                                lineHeight *= 100
                                this.hasLineHeightMultiplier = true
                            }
                            type = 1
                        } else {
                            lineHeight = 0
                            type = 0
                        }
                    }
                    if(this.hasOwnLineHeightType) type = bf.lineHeightType || 0
                    bf.lineHeight = lineHeight
                    bf.lineHeightType = type
                    break
                }
                case 'text-indent': {
                    let indent = Css.realValue(decl, 'px')
                    if(indent !== null) bf.textIndent = indent
                    break
                }
                case '-qt-list-indent': {
                    let v = Css.intValue(decl, null)
                    if(v !== null){
                        this.cssListIndent = v
                        this.hasCssListIndent = true
                    }
                    break
                }
                case '-qt-paragraph-type':
                    if(String(first.variant).toLowerCase() === 'empty') this.isEmptyParagraph = true
                    break
                case '-qt-table-type': {
                    let v = String(first.variant).toLowerCase()
                    if(v === 'frame') this.isTextFrame = true
                    else if(v === 'root'){
                        this.isTextFrame = true
                        this.isRootFrame = true
                    }
                    break
                }
                case '-qt-user-state': this.userState = Math.trunc(Number(first.variant)) || 0; break
                case 'white-space':
                    switch(identifier){
                        case 'normal': this.wsm = WS_NORMAL; break
                        case 'pre': this.wsm = WS_PRE; break
                        case 'nowrap': this.wsm = WS_NOWRAP; break
                        case 'pre-wrap': this.wsm = WS_PREWRAP; break
                        case 'pre-line': this.wsm = WS_PRELINE; break
                    }
                    break
                case 'vertical-align':
                    cf.verticalAlignment = identifier === 'sub' ? F.ALIGN_SUBSCRIPT : identifier === 'super' ? F.ALIGN_SUPERSCRIPT
                        : identifier === 'middle' ? F.ALIGN_MIDDLE : identifier === 'top' ? F.ALIGN_TOP
                        : identifier === 'bottom' ? F.ALIGN_BOTTOM : F.ALIGN_NORMAL
                    break
                case 'page-break-before':
                    if(identifier === 'always') bf.pageBreakPolicy = (bf.pageBreakPolicy || 0) | 0x1
                    else if(identifier === 'auto') bf.pageBreakPolicy = (bf.pageBreakPolicy || 0) & ~0x1
                    break
                case 'page-break-after':
                    if(identifier === 'always') bf.pageBreakPolicy = (bf.pageBreakPolicy || 0) | 0x2
                    else if(identifier === 'auto') bf.pageBreakPolicy = (bf.pageBreakPolicy || 0) & ~0x2
                    break
                case 'text-underline-style': {
                    let map = { 'none': 0, 'solid': 1, 'dashed': 2, 'dotted': 3, 'dot-dash': 4, 'dot-dot-dash': 5, 'wave': 6 }
                    if(identifier in map){
                        cf.underlineStyle = map[identifier]
                        cf.fontUnderline = map[identifier] === 1
                    }
                    break
                }
                case 'text-decoration-color': {
                    let color = Css.declarationColor(decl)
                    cf.underlineColor = color && color !== 'invalid' ? color : '#000000'
                    break
                }
                case 'list-style-type': case 'list-style':
                    this.charFormat = Object.freeze(cf)
                    this.blockFormat = Object.freeze(bf)
                    this.setListStyle(decl.values)
                    cf = Object.assign({}, this.charFormat)
                    bf = Object.assign({}, this.blockFormat)
                    break
                case '-qt-list-number-prefix': this.textListNumberPrefix = String(first.variant); break
                case '-qt-list-number-suffix': this.textListNumberSuffix = String(first.variant); break
                case 'text-align':
                    if(identifier === 'left') bf.alignment = F.ALIGN_LEFT
                    else if(identifier === 'center') bf.alignment = F.ALIGN_CENTER
                    else if(identifier === 'right') bf.alignment = F.ALIGN_RIGHT
                    break
                case '-qt-foreground': {
                    let brush = Css.declarationBrush(decl)
                    if(brush) cf.foreground = brush
                    break
                }
                case 'max-width':
                    if(this.id === 'img'){
                        let data = Css.lengthData(first)
                        cf.imageMaxWidth = data.unit === '%' ? { type: 'percent', value: data.number } : { type: 'fixed', value: Css.lengthFromData(data, null) }
                    }
                    break
            }
        }
        let { font, adjustment } = Css.extractFont(declarations)
        if(font.pixelSize > 0x3fffffff) font.pixelSize = 0x3fffffff
        let next = F.setFont(Object.freeze(cf), font)
        if(adjustment >= -1) next = F.with_(next, { sizeAdjustment: adjustment })
        let background = Css.extractBackground(declarations)
        if(background.image){
            next = F.with_(next, { backgroundImageUrl: background.image })
        } else if(background.brush){
            next = F.with_(next, { background: background.brush })
            if(this.id === 'hr') bf.background = background.brush
        }
        this.charFormat = next
        this.blockFormat = Object.freeze(bf)
    }

    parseStyleAttribute(value){
        this.applyCssDeclarations(Css.parseDeclarations(value))
    }
}

// the width / height attribute of a table, a cell or a rule: a number of pixels or a percentage
function widthAttribute(value){
    let n = Number(value)
    if(value.trim() !== '' && !Number.isNaN(n)) return { type: 'fixed', value: n }
    let v = value.trim()
    if(v.endsWith('%')){
        let p = Number(v.slice(0, -1))
        if(v.length > 1 && !Number.isNaN(p)) return { type: 'percent', value: p }
    }
    return null
}

// QString::toInt / toDouble: the whole string must be a number
const intOf = (value) => /^\s*[+-]?\d+\s*$/.test(value) ? parseInt(value, 10) : null
const realOf = (value) => value.trim() !== '' && !Number.isNaN(Number(value)) ? Number(value) : null

class Parser {
    constructor(){
        this.nodes = []
        this.txt = ''
        this.pos = 0
        this.len = 0
        this.textEditMode = false
        this.inlineStyleSheets = []
    }

    at(i){ return this.nodes[i] }
    count(){ return this.nodes.length }
    last(){ return this.nodes.length - 1 }

    depth(i){
        let depth = 0
        while(i){
            i = this.at(i).parent
            depth++
        }
        return depth
    }

    margin(i, mar){
        let m = 0
        if(mar === MARGIN_LEFT || mar === MARGIN_RIGHT){
            while(i){
                let node = this.at(i)
                if(!node.isBlock() && node.id !== 'table') break
                if(node.isTableCell()) break
                m += node.margin[mar]
                i = node.parent
            }
        }
        return m
    }

    topMargin(i){ return i ? this.at(i).margin[MARGIN_TOP] : 0 }
    bottomMargin(i){ return i ? this.at(i).margin[MARGIN_BOTTOM] : 0 }
    leftMargin(i){ return this.margin(i, MARGIN_LEFT) }
    rightMargin(i){ return this.margin(i, MARGIN_RIGHT) }
    topPadding(i){ return this.at(i).padding[MARGIN_TOP] }
    bottomPadding(i){ return this.at(i).padding[MARGIN_BOTTOM] }
    leftPadding(i){ return this.at(i).padding[MARGIN_LEFT] }
    rightPadding(i){ return this.at(i).padding[MARGIN_RIGHT] }

    hasPrefix(c, lookahead = 0){
        return this.pos + lookahead < this.len && this.txt[this.pos + lookahead] === c
    }

    newNode(parent){
        let lastNode = this.nodes[this.nodes.length - 1]
        let reuse = true
        if(this.nodes.length === 1) reuse = false
        else if(lastNode.tag === ''){
            if(lastNode.text === '') reuse = true
            else if(lastNode.text.length === 1 && isSpace(lastNode.text[0])){
                let lastSibling = this.count() - 2
                while(lastSibling && this.at(lastSibling).parent !== lastNode.parent && this.at(lastSibling).displayMode === INLINE){
                    lastSibling = this.at(lastSibling).parent
                }
                reuse = this.at(lastSibling).displayMode !== INLINE
            } else reuse = false
        } else reuse = false
        let node
        if(reuse){
            node = lastNode
            node.tag = ''
            node.text = ''
            node.id = null
        } else {
            node = new Node()
            this.nodes.push(node)
        }
        node.parent = parent
        return node
    }

    parse(text){
        this.nodes = [new Node()]
        this.txt = text
        this.pos = 0
        this.len = text.length
        this.textEditMode = false
        this.inlineStyleSheets = []
        while(this.pos < this.len){
            let c = this.txt[this.pos++]
            if(c === '<') this.parseTag()
            else if(c === '&') this.nodes[this.nodes.length - 1].text += this.parseEntity()
            else this.nodes[this.nodes.length - 1].text += c
        }
    }

    eatSpace(){
        while(this.pos < this.len && isSpace(this.txt[this.pos]) && this.txt[this.pos] !== '\u2029') this.pos++
    }

    parseTag(){
        this.eatSpace()
        // comments and other declarations
        if(this.hasPrefix('!')){
            this.parseExclamationTag()
            let wsm = this.nodes[this.nodes.length - 1].wsm
            if(wsm !== WS_PRE && wsm !== WS_PREWRAP && !this.textEditMode) this.eatSpace()
            return
        }
        if(this.hasPrefix('/')){
            let lastNode = this.nodes[this.nodes.length - 1]
            if(lastNode.id === 'style') this.inlineStyleSheets.push(Css.parseStyleSheet(lastNode.text))
            this.parseCloseTag()
            return
        }
        let p = this.last()
        while(p && this.at(p).tag.length === 0) p = this.at(p).parent
        let node = this.newNode(p)
        node.tag = this.parseWord().toLowerCase()
        let display = Object.prototype.hasOwnProperty.call(ELEMENTS, node.tag) ? ELEMENTS[node.tag] : undefined
        if(display !== undefined){
            node.id = node.tag === 'qt' ? 'body' : node.tag
            node.displayMode = display
        } else node.id = null
        node.attributes = []
        // at least one space after the name for attributes
        if(this.pos < this.len && isSpace(this.txt[this.pos])) node.attributes = this.parseAttributes()
        node = this.resolveParent()
        this.resolveNode()
        node.applyCssDeclarations(this.declarationsForNode(this.nodes.length - 1))
        this.applyAttributes(node.attributes)
        let tagClosed = false
        while(this.pos < this.len && this.txt[this.pos] !== '>'){
            if(this.txt[this.pos] === '/') tagClosed = true
            this.pos++
        }
        this.pos++
        // a white space preserving block drops a newline right after its tag
        if((node.wsm === WS_PRE || node.wsm === WS_PREWRAP || node.wsm === WS_PRELINE) && node.isBlock()){
            if(this.pos < this.len - 1 && this.txt[this.pos] === '\n') this.pos++
        }
        if(node.mayNotHaveChildren() || tagClosed){
            this.newNode(node.parent)
            this.resolveNode()
        }
    }

    parseCloseTag(){
        this.pos++
        let tag = this.parseWord().toLowerCase().trim()
        while(this.pos < this.len){
            let c = this.txt[this.pos++]
            if(c === '>') break
        }
        let p = this.last()
        if(p > 0 && this.at(p - 1).tag === tag && this.at(p - 1).mayNotHaveChildren()) p--
        while(p && this.at(p).tag !== tag) p = this.at(p).parent
        // a close tag without its open tag is ignored
        if(!p) return
        // ...foo\n</pre> is foo</pre>
        let closed = this.at(p)
        if((closed.wsm === WS_PRE || closed.wsm === WS_PREWRAP || closed.wsm === WS_PRELINE) && closed.isBlock()){
            let lastNode = this.nodes[this.last()]
            if(lastNode.text.endsWith('\n')) lastNode.text = lastNode.text.slice(0, -1)
        }
        this.newNode(closed.parent)
        this.resolveNode()
    }

    parseExclamationTag(){
        this.pos++
        if(this.hasPrefix('-') && this.hasPrefix('-', 1)){
            this.pos += 2
            let end = this.txt.indexOf('-->', this.pos)
            this.pos = end >= 0 ? end + 3 : this.len
        } else {
            while(this.pos < this.len){
                let c = this.txt[this.pos++]
                if(c === '>') break
            }
        }
    }

    static entityText(entity){
        if(Object.prototype.hasOwnProperty.call(ENTITIES, entity)) return String.fromCharCode(ENTITIES[entity])
        if(entity.length > 1 && entity[0] === '#'){
            let body = entity.slice(1)
            let base = 10
            if(body[0] && body[0].toLowerCase() === 'x'){
                body = body.slice(1)
                base = 16
            }
            if(body !== '' && (base === 16 ? /^[0-9a-fA-F]+$/ : /^[0-9]+$/).test(body)){
                let uc = parseInt(body, base)
                if(uc <= 0xffffffff){
                    if(uc >= 0x80 && uc < 0x80 + WINDOWS_LATIN1.length) uc = WINDOWS_LATIN1[uc - 0x80]
                    if(uc > 0x10ffff) return '�'
                    return String.fromCodePoint(uc)
                }
            }
        }
        return null
    }

    // an entity after '&'; "&" itself when it is none
    parseEntity(){
        let recover = this.pos
        let entityLen = 0
        let error = false
        while(this.pos < this.len){
            let c = this.txt[this.pos++]
            if(isSpace(c) || this.pos - recover > 9){
                error = true
                break
            }
            if(c === ';') break
            entityLen++
        }
        if(!error && entityLen){
            let parsed = Parser.entityText(this.txt.substr(recover, entityLen))
            if(parsed !== null) return parsed
        }
        this.pos = recover
        return '&'
    }

    // a word, possibly quoted
    parseWord(){
        let word = ''
        if(this.hasPrefix('"')){
            this.pos++
            while(this.pos < this.len){
                let c = this.txt[this.pos++]
                if(c === '"') break
                else if(c === '&') word += this.parseEntity()
                else word += c
            }
        } else if(this.hasPrefix("'")){
            this.pos++
            while(this.pos < this.len){
                let c = this.txt[this.pos++]
                if(c === "'" && (this.txt.length > 1 && this.txt[this.pos - 2] !== '\\')) break
                else word += c
            }
        } else {
            while(this.pos < this.len){
                let c = this.txt[this.pos++]
                if(c === '>' || (c === '/' && this.hasPrefix('>')) || c === '<' || c === '=' || isSpace(c)){
                    this.pos--
                    break
                }
                if(c === '&') word += this.parseEntity()
                else word += c
            }
        }
        return word
    }

    // the right parent of the new node: tables made for a lone tr or td, a paragraph closing inline elements, elements
    // that do not nest in themselves or in some contexts
    resolveParent(){
        let node = this.nodes[this.nodes.length - 1]
        let p = node.parent
        if(node.id === 'td'){
            let n = p
            while(n && this.at(n).id !== 'tr') n = this.at(n).parent
            if(!n){
                let table = new Node(), row = new Node()
                this.nodes.splice(this.nodes.length - 1, 0, table, row)
                table.parent = p
                table.id = 'table'
                table.tag = 'table'
                table.displayMode = TABLE
                table.children.push(this.nodes.length - 2)
                row.parent = this.nodes.length - 3
                row.id = 'tr'
                row.tag = 'tr'
                row.displayMode = TABLE
                p = this.nodes.length - 2
                node = this.nodes[this.nodes.length - 1]
            }
        }
        if(node.id === 'tr'){
            let n = p
            while(n && this.at(n).id !== 'table') n = this.at(n).parent
            if(!n){
                let table = new Node()
                this.nodes.splice(this.nodes.length - 1, 0, table)
                table.parent = p
                table.id = 'table'
                table.tag = 'table'
                table.displayMode = TABLE
                p = this.nodes.length - 2
                node = this.nodes[this.nodes.length - 1]
            }
        }
        if(node.id === 'p'){
            while(p && !this.at(p).isBlock()) p = this.at(p).parent
            if(!p || this.at(p).id !== 'p') p = node.parent
        }
        if(node.id !== null && node.id === this.at(p).id && node.isNotSelfNesting()) p = this.at(p).parent
        while((p && !node.allowedInContext(this.at(p).id)) || this.at(p).mayNotHaveChildren()) p = this.at(p).parent
        node.parent = p
        this.nodes[p].children.push(this.nodes.length - 1)
        return node
    }

    resolveNode(){
        let node = this.nodes[this.nodes.length - 1]
        node.initializeProperties(this.nodes[node.parent], this)
    }

    parseAttributes(){
        let attrs = []
        while(this.pos < this.len){
            this.eatSpace()
            if(this.hasPrefix('>') || this.hasPrefix('/')) break
            let key = this.parseWord().toLowerCase()
            let value = '1'
            if(key.length === 0) break
            this.eatSpace()
            if(this.hasPrefix('=')){
                this.pos++
                this.eatSpace()
                value = this.parseWord()
            }
            if(value.length === 0) continue
            attrs.push(key, value)
        }
        return attrs
    }

    applyAttributes(attributes){
        let seenQt3Richtext = false
        if(attributes.length % 2 === 1) return
        let node = this.nodes[this.nodes.length - 1]
        let set = (changes) => { node.charFormat = F.with_(node.charFormat, changes) }
        let setBlock = (changes) => { node.blockFormat = F.with_(node.blockFormat, changes) }
        let color = (value) => {
            let c = Css.colorOf({ type: 'String', variant: value })
            return c && c !== 'invalid' ? c : null
        }
        for(let i = 0; i < attributes.length; i += 2){
            let key = attributes[i]
            let value = attributes[i + 1]
            switch(node.id){
                case 'font':
                    if(key === 'size' && value.length){
                        let n = intOf(value) || 0
                        if(value[0] !== '+' && value[0] !== '-') n -= 3
                        set({ sizeAdjustment: n })
                    } else if(key === 'face'){
                        set({ families: value.includes(',') ? value.split(',').map(s => s.trim()) : [value] })
                    } else if(key === 'color'){
                        // an invalid color is an invalid brush: black
                        set({ foreground: color(value) || '#000000' })
                    }
                    break
                case 'ol': case 'ul':
                    if(key === 'type'){
                        node.hasOwnListStyle = true
                        if(value === '1') node.listStyle = F.LIST_DECIMAL
                        else if(value === 'a') node.listStyle = F.LIST_LOWER_ALPHA
                        else if(value === 'A') node.listStyle = F.LIST_UPPER_ALPHA
                        else if(value === 'i') node.listStyle = F.LIST_LOWER_ROMAN
                        else if(value === 'I') node.listStyle = F.LIST_UPPER_ROMAN
                        else {
                            let lower = value.toLowerCase()
                            if(lower === 'square') node.listStyle = F.LIST_SQUARE
                            else if(lower === 'disc') node.listStyle = F.LIST_DISC
                            else if(lower === 'circle') node.listStyle = F.LIST_CIRCLE
                            else if(lower === 'none') node.listStyle = F.LIST_UNDEFINED
                        }
                    } else if(key === 'start'){
                        let n = intOf(value)
                        if(n !== null) node.listStart = n
                    }
                    break
                case 'li':
                    if(key === 'class'){
                        if(value === 'unchecked') setBlock({ marker: 1 })
                        else if(value === 'checked') setBlock({ marker: 2 })
                    }
                    break
                case 'a':
                    if(key === 'href') set({ anchorHref: value })
                    else if(key === 'name') set({ anchorNames: [value] })
                    break
                case 'img':
                    if(key === 'src' || key === 'source') node.imageName = value
                    else if(key === 'width'){
                        node.imageWidth = -2
                        let n = realOf(value)
                        if(n !== null) node.imageWidth = n
                    } else if(key === 'height'){
                        node.imageHeight = -2
                        let n = realOf(value)
                        if(n !== null) node.imageHeight = n
                    } else if(key === 'alt') node.imageAlt = value
                    else if(key === 'title') node.text = value
                    break
                case 'tr': case 'body':
                    if(key === 'bgcolor') set({ background: color(value) || '#000000' })
                    else if(key === 'background') set({ backgroundImageUrl: value })
                    break
                case 'th': case 'td':
                    if(key === 'width'){
                        let w = widthAttribute(value)
                        if(w) node.width = w
                    } else if(key === 'bgcolor') set({ background: color(value) || '#000000' })
                    else if(key === 'background') set({ backgroundImageUrl: value })
                    else if(key === 'rowspan'){
                        let n = intOf(value)
                        if(n !== null) node.tableCellRowSpan = Math.max(1, n)
                    } else if(key === 'colspan'){
                        let n = intOf(value)
                        if(n !== null) node.tableCellColSpan = Math.max(1, Math.min(n, 20480))
                    }
                    break
                case 'table':
                    if(key === 'border' && !node.tableBorder){
                        let n = realOf(value)
                        if(n !== null) node.tableBorder = n
                    } else if(key === 'bgcolor') set({ background: color(value) || '#000000' })
                    else if(key === 'bordercolor') node.borderBrush = color(value) || '#000000'
                    else if(key === 'background') set({ backgroundImageUrl: value })
                    else if(key === 'cellspacing'){
                        let n = realOf(value)
                        if(n !== null) node.tableCellSpacing = n
                    } else if(key === 'cellpadding'){
                        let n = realOf(value)
                        if(n !== null) node.tableCellPadding = n
                    } else if(key === 'width'){
                        let w = widthAttribute(value)
                        if(w) node.width = w
                    } else if(key === 'height'){
                        let w = widthAttribute(value)
                        if(w) node.height = w
                    }
                    break
                case 'meta':
                    if(key === 'name' && value === 'qrichtext') seenQt3Richtext = true
                    if(key === 'content' && value === '1' && seenQt3Richtext) this.textEditMode = true
                    break
                case 'hr':
                    if(key === 'width'){
                        let w = widthAttribute(value)
                        if(w) node.width = w
                    }
                    break
                case 'pre':
                    if(key === 'class' && value.startsWith('language-')) setBlock({ codeLanguage: value.slice(9) })
                    break
            }
            if(key === 'style'){
                node.parseStyleAttribute(value)
            } else if(key === 'align'){
                let lower = value.toLowerCase()
                let alignment = null
                if(lower === 'left') alignment = F.ALIGN_LEFT | F.ALIGN_ABSOLUTE
                else if(lower === 'right') alignment = F.ALIGN_RIGHT | F.ALIGN_ABSOLUTE
                else if(lower === 'center') alignment = F.ALIGN_HCENTER
                else if(lower === 'justify') alignment = F.ALIGN_JUSTIFY
                if(alignment !== null) setBlock({ alignment })
                if(node.id === 'img'){
                    if(alignment !== null){
                        if(alignment & F.ALIGN_LEFT) node.cssFloat = 'left'
                        else if(alignment & F.ALIGN_RIGHT) node.cssFloat = 'right'
                    } else if(lower === 'middle') set({ verticalAlignment: F.ALIGN_MIDDLE })
                    else if(lower === 'top') set({ verticalAlignment: F.ALIGN_TOP })
                }
            } else if(key === 'valign'){
                let lower = value.toLowerCase()
                if(lower === 'top') set({ verticalAlignment: F.ALIGN_TOP })
                else if(lower === 'middle') set({ verticalAlignment: F.ALIGN_MIDDLE })
                else if(lower === 'bottom') set({ verticalAlignment: F.ALIGN_BOTTOM })
            } else if(key === 'dir'){
                let lower = value.toLowerCase()
                if(lower === 'ltr') setBlock({ layoutDirection: 0 })
                else if(lower === 'rtl') setBlock({ layoutDirection: 1 })
            } else if(key === 'title'){
                set({ toolTip: value })
            } else if(key === 'id'){
                set({ isAnchor: true, anchorNames: [value] })
            }
        }
    }

    // the declarations of the element itself (standardDeclarationForNode), of the style sheets for it and the
    // inheritable ones of the style sheets for its ancestors (QTextHtmlParser::declarationsForNode)
    declarationsForNode(index){
        let node = this.nodes[index]
        let decls = standardDeclarations(node)
        if(!this.inlineStyleSheets.length) return decls
        let parser = this
        let adapter = {
            name: (i) => parser.at(i).tag.toLowerCase(),
            attribute: (i, name) => {
                let attributes = parser.at(i).attributes
                for(let k = 0; k < attributes.length; k += 2) if(attributes[k] === name) return attributes[k + 1]
                return null
            },
            parent: (i) => i ? parser.at(i).parent : 0,
            previousSibling: (i) => {
                if(!i) return 0
                let parent = parser.at(i).parent
                if(!parent) return 0
                let siblings = parser.at(parent).children
                let at = siblings.indexOf(i)
                return at <= 0 ? 0 : siblings[at - 1]
            },
            isNull: (i) => i === 0,
        }
        let extraPseudo = node.id === 'a' && node.hasHref ? 'link' : null
        decls = decls.concat(Css.declarationsForNode(this.inlineStyleSheets, index, adapter, extraPseudo))
        for(let n = adapter.parent(index); n; n = adapter.parent(n)){
            let inherited = Css.declarationsForNode(this.inlineStyleSheets, n, adapter, extraPseudo)
            for(let decl of inherited) if(decl.inheritable) decls.unshift(decl)
        }
        return decls
    }
}

const known = (variant) => ({ type: 'KnownIdentifier', variant })

// standardDeclarationForNode
function standardDeclarations(node){
    let decls = []
    let decl = (property, values) => decls.push(Css.makeDeclaration(property, values, false))
    switch(node.id){
        case 'a': case 'u': {
            let underline = node.id === 'u'
            if(node.id === 'a'){
                for(let i = 0; i < node.attributes.length; i += 2){
                    if(node.attributes[i].toLowerCase() === 'href' && node.attributes[i + 1] !== ''){
                        underline = true
                        decl('color', [{ type: 'Function', variant: ['palette', 'link'] }])
                        break
                    }
                }
            }
            if(underline) decl('text-decoration', [known('underline')])
            break
        }
        case 'b': case 'strong': case 'h1': case 'h2': case 'h3': case 'h4': case 'h5': case 'th':
            decl('font-weight', [known('bold')])
            if(node.id === 'b' || node.id === 'strong') break
            if(node.id !== 'th'){
                let size = { h1: 'xx-large', h2: 'x-large', h3: 'large', h4: 'medium', h5: 'small' }[node.id]
                decls.push(Object.assign(Css.makeDeclaration('font-size', [known(size)], false), { inheritable: false }))
                break
            }
            decl('text-align', [known('center')])
            break
        case 'big': case 'small':
            decls.push(Object.assign(Css.makeDeclaration('font-size', [known(node.id === 'big' ? 'large' : 'small')], false), { inheritable: false }))
            break
        case 'center': case 'td':
            decl('text-align', [known(node.id === 'td' ? 'left' : 'center')])
            break
        case 'del': case 's':
            decl('text-decoration', [known('line-through')])
            break
        case 'em': case 'i': case 'cite': case 'address': case 'var': case 'dfn':
            decl('font-style', [known('italic')])
            break
        case 'sub': case 'sup':
            decl('vertical-align', [known(node.id === 'sub' ? 'sub' : 'super')])
            break
        case 'ul': case 'ol':
            decl('list-style', [known(node.id === 'ul' ? 'disc' : 'decimal')])
            break
        case 'code': case 'tt': case 'kbd': case 'samp': case 'pre':
            decl('font-family', [{ type: 'String', variant: FIXED_FAMILY }])
            if(node.id === 'pre') decl('white-space', [known('pre')])
            break
        case 'br':
            decl('white-space', [known('pre-wrap')])
            break
        case 'nobr':
            decl('white-space', [known('nowrap')])
            break
    }
    return decls
}

module.exports = { Parser, Node, isSpace, BLOCK, INLINE, TABLE, NONE, WS_NORMAL, WS_PRE, WS_NOWRAP, WS_PREWRAP, WS_PRELINE, MARGIN_TOP, MARGIN_RIGHT, MARGIN_BOTTOM, MARGIN_LEFT }
