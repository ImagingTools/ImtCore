// Markdown of a text document: QTextMarkdownImporter (qtextmarkdownimporter.cpp of Qt 6.12) makes the blocks, lists,
// tables and formats of the parser's events (MarkdownParser.js reports them as md4c does); QTextMarkdownWriter
// (qtextmarkdownwriter.cpp) writes a document back as Markdown. Both with the features of
// QTextDocument::MarkdownDialectGitHub (and the front matter, kept by the document)
const F = require('./Formats')
const { TextDocument, BEGINNING_OF_FRAME, END_OF_FRAME, LINE_SEPARATOR } = require('./Document')
const { TextCursor, MOVE } = require('./Cursor')
const { fragmentFromHtml, insertFragment } = require('./Fragment')
const MD = require('./MarkdownParser')

const { BLOCK, SPAN, TEXT, ALIGN } = MD

// qtmi_BlockQuoteIndent
const BLOCK_QUOTE_INDENT = 40
// hard safety limit for generated quote prefixes to avoid unbounded allocations
const MAX_BLOCK_QUOTE_LEVEL = 128
// QPalette::link of the default palette
const LINK_COLOR = '#0000ff'
// QFontDatabase::systemFont(QFontDatabase::FixedFont)
const MONO_FAMILY = 'monospace'
const MONO_FAMILIES = new Set(['monospace', 'courier', 'courier new', 'menlo', 'monaco', 'consolas', 'dejavu sans mono',
    'liberation mono', 'lucida console', 'andale mono', 'ubuntu mono', 'noto mono', 'sf mono', 'cascadia code', 'cascadia mono'])

// QTextBlockFormat::MarkerType
const NO_MARKER = 0, UNCHECKED = 1, CHECKED = 2

// QTextCharFormat::setFont(the fixed font): every font property of it, the size left to the document's default font
function monoFormat(format){
    return F.with_(format, {
        families: [MONO_FAMILY], weight: 400, italic: false, underlineStyle: F.NO_UNDERLINE, fontUnderline: false, overline: false,
        strikeOut: false, capitalization: 0, wordSpacing: 0, letterSpacingType: 'percent', letterSpacing: 0, kerning: true,
        fixedPitch: true, pointSize: undefined, pixelSize: undefined,
    })
}

// QFontInfo::fixedPitch of the format's font
function isMono(format){
    if(format.fixedPitch) return true
    let families = format.families || []
    return families.length > 0 && MONO_FAMILIES.has(String(families[0]).toLowerCase())
}

// QTextTableFormat()
const TABLE_FORMAT = Object.freeze({ cellPadding: 4, borderCollapse: true, border: 1, borderStyle: 'outset', borderBrush: '#808080' })

// md4c's alignment of a cell as Qt::Alignment (MdAlignment)
function cellAlignment(align){
    const V_CENTER = 0x80
    if(align === ALIGN.CENTER) return F.ALIGN_HCENTER | V_CENTER
    if(align === ALIGN.RIGHT) return F.ALIGN_RIGHT | V_CENTER
    return F.ALIGN_LEFT | V_CENTER
}

// splitFrontMatter: a document starting with a line '---' up to the next such line
function splitFrontMatter(md){
    let marker = md.startsWith('---\n') ? '---\n' : md.startsWith('---\r\n') ? '---\r\n' : null
    if(!marker) return null
    let end = md.indexOf(marker, marker.length)
    if(end < 0 || md[end - 1] !== '\n') return null
    return { frontMatter: md.slice(marker.length, end), rest: md.slice(end + marker.length) }
}

class MarkdownImporter {
    // font: the default font of the document ({ pointSize, sizeUnit }); the margin of a paragraph is 2/3 of its size in
    // points (0 for a font in pixels: QFont::pointSize is -1)
    constructor(doc, font){
        this.doc = doc
        this.cursor = new TextCursor(doc, 0)
        this.font = font || null
    }

    import(markdown){
        let doc = this.doc
        let pointSize = this.font && this.font.sizeUnit !== 'px' && this.font.pointSize > 0 ? Math.round(this.font.pointSize) : -1
        this.paragraphMargin = Math.trunc(pointSize * 2 / 3) || 0
        doc.clear()
        this.cursor = new TextCursor(doc, 0)
        this.blockType = BLOCK.DOC
        this.needsInsertBlock = false
        this.needsInsertList = false
        this.listItem = false
        this.listStack = []
        this.listFormat = null
        this.markerType = NO_MARKER
        this.blockQuoteDepth = 0
        this.codeBlock = false
        this.codeLanguage = ''
        this.codeFence = ''
        this.spanStack = []
        this.imageSpan = false
        this.imageFormat = null
        this.htmlTagDepth = 0
        this.htmlAccumulator = ''
        this.table = null
        let md = String(markdown)
        let split = splitFrontMatter(md)
        if(split){
            doc.frontMatter = split.frontMatter
            md = split.rest
        }
        doc.beginEditBlock()
        try {
            MD.parse(md, this)
        } finally {
            doc.endEditBlock()
        }
    }

    top(){
        return this.spanStack.length ? this.spanStack[this.spanStack.length - 1] : F.EMPTY
    }

    // QTextCursor::insertList: a block, a list of it
    insertList(format){
        this.cursor.insertBlock()
        return this.cursor.createList(format)
    }

    // QTextList::add: the block one of the list's items
    addToList(list, block){
        let format = this.doc.blockFormat(block)
        if(format.list !== list) this.doc.setBlockFormat(block, F.with_(format, { list }))
    }

    tableCell(row, column){
        let entry = this.doc.tables().find(e => e.table === this.table.table)
        let grid = this.doc.tableGrid(entry)
        let cell = grid.cellAt(row, column)
        return cell && cell.row === row && cell.column === column ? cell : null
    }

    enterBlock(type, detail){
        this.blockType = type
        switch(type){
            case BLOCK.P:
                this.needsInsertBlock = true
                break
            case BLOCK.QUOTE:
                this.blockQuoteDepth++
                break
            case BLOCK.CODE:
                this.codeBlock = true
                this.codeLanguage = detail.lang || ''
                this.codeFence = detail.fenceChar || ''
                this.needsInsertBlock = true
                break
            case BLOCK.H: {
                let blockFormat = Object.freeze({ headingLevel: detail.level })
                let charFormat = Object.freeze({ sizeAdjustment: 4 - detail.level, weight: 700 })
                this.needsInsertBlock = false
                if(this.doc.isEmpty()){
                    this.cursor.setBlockFormat(blockFormat)
                    this.cursor.setCharFormat(charFormat)
                } else {
                    this.cursor.insertBlock(blockFormat, charFormat)
                }
                break
            }
            case BLOCK.LI:
                this.needsInsertBlock = true
                this.listItem = true
                this.markerType = detail.isTask ? (detail.taskMark === ' ' ? UNCHECKED : CHECKED) : NO_MARKER
                break
            case BLOCK.UL: case BLOCK.OL: {
                // a list nested in an empty item
                if(this.needsInsertList) this.listStack.push(this.insertList(this.listFormat))
                else this.needsInsertList = true
                let format = { indent: this.listStack.length + 1 }
                if(type === BLOCK.UL){
                    format.style = detail.mark === '*' ? F.LIST_CIRCLE : detail.mark === '+' ? F.LIST_SQUARE : F.LIST_DISC
                } else {
                    format.numberSuffix = detail.markDelimiter
                    format.style = F.LIST_DECIMAL
                    format.start = detail.start
                }
                this.listFormat = Object.freeze(format)
                break
            }
            case BLOCK.TABLE: {
                // the size the parser knows (Qt grows a table of 1 x 1 as the rows and the cells come)
                let rows = detail.headRows + detail.bodyRows
                this.table = { table: null, rowCount: 0, columnCount: 0, column: -1, nonEmpty: [] }
                this.table.table = this.cursor.insertTable(rows, detail.columns, TABLE_FORMAT)
                break
            }
            case BLOCK.TR:
                this.table.rowCount++
                this.table.nonEmpty = []
                this.table.column = -1
                break
            case BLOCK.TH: case BLOCK.TD: {
                let t = this.table
                if(type === BLOCK.TH) t.columnCount++
                t.column++
                let cell = this.tableCell(t.rowCount - 1, t.column)
                if(!cell){
                    console.warn('JQML: malformed table in Markdown input')
                    return
                }
                this.cursor = new TextCursor(this.doc, cell.first)
                if(type === BLOCK.TH){
                    this.doc.setCellFormat(cell.marker, F.with_(this.doc.charFormatAt(cell.marker), { weight: 700 }))
                } else {
                    this.cursor.setBlockFormat(F.with_(this.cursor.blockFormat(), { alignment: cellAlignment(detail.align) }))
                }
                break
            }
            case BLOCK.HR: {
                // BlockTrailingHorizontalRulerWidth 1: no length (QTextLength::VariableLength)
                this.cursor.insertBlock(Object.freeze({ hrWidth: { type: 'variable', value: 0 } }), F.EMPTY)
                break
            }
        }
    }

    leaveBlock(type){
        switch(type){
            case BLOCK.P:
                this.listItem = false
                break
            case BLOCK.UL: case BLOCK.OL:
                if(this.needsInsertList) this.listStack.push(this.cursor.createList(this.listFormat))
                if(this.listStack.length) this.listStack.pop()
                break
            case BLOCK.TR: {
                // md4c does not tell which cells are merged: the empty cells after a cell that is not are merged with it
                let t = this.table
                let mergeEnd = -1, mergeBegin = -1
                for(let col = t.column; col >= 0; col--){
                    if(t.nonEmpty.includes(col)){
                        if(mergeEnd >= 0 && mergeBegin >= 0) this.doc.mergeCells(t.table, t.rowCount - 1, mergeBegin - 1, 1, mergeEnd - mergeBegin + 2)
                        mergeEnd = -1
                        mergeBegin = -1
                    } else if(mergeEnd < 0) mergeEnd = col
                    else mergeBegin = col
                }
                break
            }
            case BLOCK.QUOTE:
                this.blockQuoteDepth--
                this.needsInsertBlock = true
                break
            case BLOCK.TABLE:
                this.table = null
                this.cursor.movePosition(MOVE.End)
                break
            case BLOCK.LI:
                this.listItem = false
                break
            case BLOCK.CODE:
                this.codeBlock = false
                this.codeLanguage = ''
                this.codeFence = ''
                this.needsInsertBlock = true
                break
            case BLOCK.H:
                this.cursor.setCharFormat(F.EMPTY)
                break
        }
    }

    enterSpan(type, detail){
        let format = this.top()
        switch(type){
            case SPAN.EM: format = F.with_(format, { italic: true }); break
            case SPAN.STRONG: format = F.with_(format, { weight: 700 }); break
            case SPAN.U: format = F.with_(format, { underlineStyle: F.SINGLE_UNDERLINE, fontUnderline: true }); break
            case SPAN.A: {
                let changes = { isAnchor: true, anchorHref: detail.href }
                if(detail.title) changes.toolTip = detail.title
                changes.foreground = LINK_COLOR
                format = F.with_(format, changes)
                break
            }
            case SPAN.IMG:
                this.imageSpan = true
                this.imageFormat = { objectType: 'image', imageName: detail.src, imageTitle: detail.title, toolTip: detail.title }
                break
            case SPAN.CODE:
                format = F.with_(monoFormat(format), { fixedPitch: true })
                break
            case SPAN.DEL: format = F.with_(format, { strikeOut: true }); break
        }
        this.spanStack.push(format)
        this.cursor.setCharFormat(format)
    }

    leaveSpan(type){
        if(this.spanStack.length) this.spanStack.pop()
        this.cursor.setCharFormat(this.top())
        if(type === SPAN.IMG) this.imageSpan = false
    }

    text(type, s){
        if(this.needsInsertBlock) this.insertBlock()
        switch(type){
            case TEXT.NORMAL:
                if(this.htmlTagDepth){
                    this.htmlAccumulator += s
                    s = ''
                }
                break
            case TEXT.NULLCHAR: s = '\ufffd'; break
            case TEXT.BR: s = '\n'; break
            case TEXT.SOFTBR: s = ' '; break
            case TEXT.CODE: break
            case TEXT.ENTITY:
                if(this.htmlTagDepth) this.htmlAccumulator += s
                else insertFragment(this.cursor, fragmentFromHtml(s))
                s = ''
                break
            case TEXT.HTML: {
                // the tags opened and closed
                this.htmlTagDepth += (s.match(/<[a-zA-Z]/g) || []).length
                this.htmlTagDepth -= (s.match(/(\/>|<\/)/g) || []).length
                this.htmlAccumulator += s
                if(!this.htmlTagDepth){
                    insertFragment(this.cursor, fragmentFromHtml(this.htmlAccumulator))
                    this.cursor.setCharFormat(this.top())
                    this.htmlAccumulator = ''
                }
                s = ''
                break
            }
        }
        if(this.blockType === BLOCK.TD && this.table){
            this.table.nonEmpty.push(this.table.column)
        } else if(this.blockType === BLOCK.CODE && s === '\n'){
            // a blank line of a code block only when something follows it
            this.needsInsertBlock = true
            s = ''
        }
        if(this.imageSpan){
            this.cursor.insertImage(Object.freeze(Object.assign({}, this.imageFormat, { imageAlt: s })))
            return
        }
        if(s) this.cursor.insertText(s)
        let format = this.cursor.blockFormat()
        // a list item is indented by its list
        if(format.list && format.indent !== 0) this.cursor.setBlockFormat(F.with_(format, { indent: 0 }))
    }

    // insertBlock: a block of the state the events left (the list, the quote, the code block of it)
    insertBlock(){
        let charFormat = this.top()
        let blockFormat = {}
        if(this.listStack.length && !this.needsInsertList && this.listItem){
            let list = this.listStack[this.listStack.length - 1]
            let items = this.doc.listItems(list)
            if(items.length) blockFormat = Object.assign({}, this.doc.blockFormat(items[items.length - 1]))
        }
        if(this.blockQuoteDepth){
            blockFormat.blockQuoteLevel = this.blockQuoteDepth
            blockFormat.leftMargin = BLOCK_QUOTE_INDENT * this.blockQuoteDepth
            blockFormat.rightMargin = BLOCK_QUOTE_INDENT
        }
        if(this.codeBlock){
            blockFormat.codeLanguage = this.codeLanguage
            if(this.codeFence) blockFormat.codeFence = this.codeFence
            charFormat = monoFormat(charFormat)
        } else {
            delete blockFormat.codeLanguage
            delete blockFormat.codeFence
            blockFormat.nonBreakableLines = false
            blockFormat.topMargin = this.paragraphMargin
            blockFormat.bottomMargin = this.paragraphMargin
        }
        if(this.markerType === NO_MARKER) delete blockFormat.marker
        else blockFormat.marker = this.markerType
        if(this.listStack.length) blockFormat.indent = this.listStack.length
        blockFormat = Object.freeze(blockFormat)
        if(this.doc.isEmpty()){
            this.cursor.setBlockFormat(blockFormat)
            this.cursor.setCharFormat(charFormat)
        } else if(this.listItem){
            this.cursor.insertBlock(blockFormat, F.EMPTY)
            this.cursor.setCharFormat(charFormat)
        } else {
            this.cursor.insertBlock(blockFormat, charFormat)
        }
        if(this.needsInsertList){
            this.listStack.push(this.cursor.createList(this.listFormat))
        } else if(this.listStack.length && this.listItem){
            this.addToList(this.listStack[this.listStack.length - 1], this.cursor.block())
        }
        this.needsInsertList = false
        this.needsInsertBlock = false
        // the next blocks of the item are its continuations
        this.listItem = false
    }
}

// QTextDocument::setMarkdown: no undo step, the undo history cleared
function setMarkdown(doc, markdown, font){
    let undo = doc.undoEnabled
    doc.undoEnabled = false
    try {
        new MarkdownImporter(doc, font).import(markdown)
    } finally {
        doc.undoEnabled = undo
    }
    doc.clearUndo()
}

// QTextDocumentFragment::fromMarkdown
function fragmentFromMarkdown(markdown, font){
    let fragment = new TextDocument()
    fragment.undoEnabled = false
    new MarkdownImporter(fragment, font).import(markdown)
    return fragment
}

// QTextCursor::insertMarkdown
function insertMarkdown(cursor, markdown, font){
    let fragment = fragmentFromMarkdown(markdown, font)
    if(markdown.startsWith('\n')) cursor.insertBlock(fragment.blockFormat(0))
    insertFragment(cursor, fragment)
    if(cursor.position < cursor.doc.length() && markdown.endsWith('\n')) cursor.insertText('\n')
}

// ---- the writer ----

const isSpace = (c) => /\s/.test(c)

// nearestWordWrapIndex: the last white space before `before`
function nearestWordWrapIndex(s, before){
    before = Math.min(before, s.length)
    for(let i = before - 1; i >= 0; i--) if(isSpace(s[i])) return i
    return -1
}

// adjacentBackticksCount: the longest run of backticks
function adjacentBackticksCount(s){
    let start = -1, ret = 0
    for(let i = 0; i < s.length; i++){
        if(s[i] === '`'){
            if(start < 0) start = i
        } else if(start >= 0){
            ret = Math.max(ret, i - start)
            start = -1
        }
    }
    if(s.length && s[s.length - 1] === '`') ret = Math.max(ret, s.length - start)
    return ret
}

// maybeEscapeFirstChar: what would start a heading, a list item or a numbered item at the start of a line
function maybeEscapeFirstChar(s){
    let trimmed = s.trim()
    if(!trimmed) return s
    let first = trimmed[0]
    if('#*+-'.includes(first)){
        let i = s.indexOf(first)
        return s.slice(0, i) + '\\' + s.slice(i)
    }
    let m = /^\d+([.)])\s/.exec(s)
    if(m) return s.slice(0, m[0].length - 2) + '\\' + s.slice(m[0].length - 2)
    return s
}

// escapeSpecialCharacters: the backslashes, and a special character that starts a word (or an HTML tag)
function escapeSpecialCharacters(s){
    s = s.replace(/\\/g, '\\\\')
    let special = /[<!*[`&]+[/\p{L}\p{N}_]/u
    let i = 0
    while(i >= 0){
        let rest = s.slice(i)
        let m = special.exec(rest)
        if(m){
            let j = i + m.index
            s = s.slice(0, j) + '\\' + s.slice(j)
            i = j + 3
        }
        let space = /\s+/.exec(s.slice(i))
        i = space ? i + space.index + 1 : -1
    }
    return s
}

// createLinkTitle: the title in double quotes, its blank lines left out
function createLinkTitle(title){
    let out = '"'
    let lines = title.split(/(?<=\r\n|\n|\r)/)
    for(let line of lines){
        if(/^[ \t]*(\r\n|\n|\r)?$/.test(line)) continue
        out += line.replace(/\\/g, '\\\\').replace(/"/g, '\\"')
    }
    return out + '"'
}

// QUrl(href, StrictMode).scheme() is not empty
function hasScheme(href){
    return /^[A-Za-z][A-Za-z0-9+.-]*:/.test(href)
}

const COLUMN_LIMIT = 80

class MarkdownWriter {
    constructor(doc){
        this.doc = doc
        this.out = ''
        this.listInfos = new Map()
        this.linePrefix = ''
        this.codeBlockFence = ''
        this.wrappedLineIndent = 0
        this.lastListIndent = 1
        this.doubleNewlineWritten = false
        this.indentedCodeBlock = false
        this.fencedCodeBlock = false
        this.linePrefixWritten = false
    }

    writeAll(){
        this.writeFrontMatter(this.doc.frontMatter || '')
        let doc = this.doc
        this.tableList = doc.tables().map(entry => Object.assign(entry, {
            firstBlock: doc.blockIndexAt(entry.start + 1),
            lastBlock: doc.blockIndexAt(entry.end),
        }))
        this.writeFrame(this.rootItems())
        return this.out
    }

    writeFrontMatter(fm){
        if(!fm) return
        this.out += '---\n' + fm
        if(!fm.endsWith('\n')) this.out += '\n'
        this.out += '---\n'
    }

    // a table starts at the block after the U+FDD0 of its first cell
    tableAt(i, ancestors){
        let doc = this.doc
        return this.tableList.find(t => t.firstBlock === i && t.start === doc.blockStart(i) - 1 && !ancestors.includes(t))
    }

    // the items of a frame (QTextFrame::iterator): its blocks and its tables, [from, to)
    items(from, to, ancestors){
        let out = []
        for(let i = from; i < to; i++){
            let entry = this.tableAt(i, ancestors)
            if(entry){
                out.push({ table: entry, ancestors: ancestors.concat([entry]) })
                i = entry.lastBlock
                continue
            }
            out.push({ block: i })
        }
        return out
    }

    rootItems(){
        return { items: this.items(0, this.doc.blocks.length, []), table: null }
    }

    // writeFrame: the blocks of a frame, a table's row by row with its cells between pipes
    writeFrame(frame){
        let doc = this.doc
        let table = frame.table
        let items = frame.items
        let tableRow = -1
        let lastWasList = false
        let widths = []
        let grid = null
        if(table){
            grid = doc.tableGrid(table)
            widths = new Array(grid.columns).fill(0)
            for(let col = 0; col < grid.columns; col++){
                for(let row = 0; row < grid.rows; row++){
                    let cell = grid.cellAt(row, col)
                    if(!cell) continue
                    let length = 0
                    let first = doc.blockIndexAt(cell.first), last = doc.blockIndexAt(cell.last)
                    for(let b = first; b <= last; b++) length += doc.blockText(b).length
                    if(cell.columnSpan === 1 && widths[col] < length) widths[col] = length
                }
            }
        }
        let cellOf = (b) => grid.cells.find(c => doc.blockStart(b) >= c.first && doc.blockStart(b) <= c.last)
        for(let k = 0; k < items.length; k++){
            let item = items[k]
            if(item.table){
                let entry = item.table
                let inner = []
                let cells = doc.tableGrid(entry).cells
                for(let cell of cells) inner = inner.concat(this.items(doc.blockIndexAt(cell.first), doc.blockIndexAt(cell.last) + 1, item.ancestors))
                this.writeFrame({ items: inner, table: entry })
                continue
            }
            let block = item.block
            let next = items[k + 1]
            let format = doc.blockFormat(block)
            let nextFormat = next && next.block !== undefined ? doc.blockFormat(next.block) : F.EMPTY
            let blockQuoteIndent = format.blockQuoteLevel || 0
            let nextBlockQuoteIndent = nextFormat.blockQuoteLevel || 0
            let nextIsDifferent = false, ending = false
            if(!next){
                nextIsDifferent = true
                ending = true
            } else if((nextFormat.indent || 0) !== (format.indent || 0) || nextFormat.codeLanguage !== format.codeLanguage){
                nextIsDifferent = true
            }
            let list = format.list || null
            if(table){
                let cell = cellOf(block)
                if(tableRow < cell.row){
                    if(tableRow === 0){
                        this.out += '\n'
                        for(let col = 0; col < widths.length; col++) this.out += '|' + '-'.repeat(widths[col])
                        this.out += '|'
                    }
                    this.out += '\n|'
                    tableRow = cell.row
                }
            } else if(!list){
                if(lastWasList){
                    this.out += '\n'
                    this.linePrefixWritten = false
                }
            }
            let endingCol = this.writeBlock(block, !table, !!table && tableRow === 0, nextIsDifferent && !list)
            this.doubleNewlineWritten = false
            if(table){
                let cell = cellOf(block)
                let padding = -endingCol
                let spanEnd = cell.column + cell.columnSpan
                for(let col = cell.column; col < spanEnd; col++) padding += widths[col]
                if(padding > 0) this.out += ' '.repeat(padding)
                for(let col = cell.column; col < spanEnd; col++) this.out += '|'
            } else if(this.fencedCodeBlock && ending){
                this.out += '\n' + this.linePrefix + ' '.repeat(this.wrappedLineIndent) + this.codeBlockFence + '\n\n'
                this.codeBlockFence = ''
            } else if(this.indentedCodeBlock && nextIsDifferent){
                this.out += '\n\n'
            } else if(endingCol > 0){
                if(list || 'codeLanguage' in format){
                    this.out += '\n'
                    if(list){
                        this.out += this.linePrefix
                        this.linePrefixWritten = true
                    }
                } else {
                    this.out += '\n'
                    if(nextBlockQuoteIndent < blockQuoteIndent) this.setLinePrefixForBlockQuote(nextBlockQuoteIndent)
                    this.out += this.linePrefix
                    this.out += '\n'
                    this.doubleNewlineWritten = true
                }
            }
            lastWasList = !!list
        }
        if(table){
            this.out += '\n\n'
            this.doubleNewlineWritten = true
        }
        this.listInfos.clear()
    }

    // listInfo: a list is loose when a block that is no item of it is between its first and its last item
    listInfo(list){
        if(this.listInfos.has(list)) return this.listInfos.get(list)
        let info = { loose: false }
        let items = this.doc.listItems(list)
        if(items.length > 1){
            let last = items[items.length - 1]
            for(let b = items[0] + 1; b < this.doc.blocks.length && b !== last; b++){
                if(!this.doc.blockFormat(b).list){
                    info.loose = true
                    break
                }
            }
        }
        this.listInfos.set(list, info)
        return info
    }

    setLinePrefixForBlockQuote(level){
        let safeLevel = Number.isFinite(level) ? Math.floor(level) : 0
        safeLevel = Math.max(0, Math.min(MAX_BLOCK_QUOTE_LEVEL, safeLevel))
        this.linePrefix = safeLevel > 0 ? '> '.repeat(safeLevel) : ''
    }

    // writeBlock: the block's prefix (quote, list item, code fence, heading), its fragments with their markers, wrapped
    // at 80 columns; the column it ends at
    writeBlock(block, wrap, ignoreFormat, ignoreEmpty){
        let doc = this.doc
        let text = doc.blockText(block)
        if(!text.length && ignoreEmpty) return 0
        let blockFmt = doc.blockFormat(block)
        let missedBlankCodeBlockLine = false
        let codeBlock = 'codeFence' in blockFmt || (blockFmt.codeLanguage || '').length > 0 || !!blockFmt.nonBreakableLines
        let blockQuoteLevel = blockFmt.blockQuoteLevel || 0
        let list = blockFmt.list || null
        if(this.fencedCodeBlock && !codeBlock){
            this.out += this.linePrefix + this.codeBlockFence + '\n'
            this.fencedCodeBlock = false
            this.codeBlockFence = ''
            this.linePrefixWritten = this.linePrefix.length > 0
        }
        this.linePrefix = ''
        if(!blockFmt.headingLevel && blockQuoteLevel > 0){
            this.setLinePrefixForBlockQuote(blockQuoteLevel)
            if(!this.linePrefixWritten){
                this.out += this.linePrefix
                this.linePrefixWritten = true
            }
        }
        if(list){
            let fmt = list.format
            let listLevel = fmt.indent || 0
            let start = 'start' in fmt && fmt.start >= 0 ? fmt.start : 1
            let number = doc.itemNumber(list, block) + start
            let bullet = ' '
            let numeric = false
            switch(fmt.style){
                case F.LIST_DISC: bullet = '-'; this.wrappedLineIndent = 2; break
                case F.LIST_CIRCLE: bullet = '*'; this.wrappedLineIndent = 2; break
                case F.LIST_SQUARE: bullet = '+'; this.wrappedLineIndent = 2; break
                case F.LIST_DECIMAL: case F.LIST_LOWER_ALPHA: case F.LIST_UPPER_ALPHA: case F.LIST_LOWER_ROMAN: case F.LIST_UPPER_ROMAN:
                    numeric = true
                    this.wrappedLineIndent = 4
                    break
            }
            if(blockFmt.marker === CHECKED) bullet += ' [x]'
            else if(blockFmt.marker === UNCHECKED) bullet += ' [ ]'
            let indentFirstLine = (listLevel - 1) * (numeric ? 4 : 2)
            this.wrappedLineIndent += indentFirstLine
            if(this.lastListIndent !== listLevel && !this.doubleNewlineWritten && this.listInfo(list).loose) this.out += '\n'
            this.lastListIndent = listLevel
            let prefix = ' '.repeat(Math.max(0, indentFirstLine))
            if(numeric){
                let suffix = fmt.numberSuffix || '.'
                let numberStr = String(number) + suffix + ' '
                if(numberStr.length === 3) numberStr += ' '
                prefix += numberStr
            } else {
                prefix += bullet + ' '
            }
            this.out += prefix
        } else if('hrWidth' in blockFmt){
            // an unambiguous rule, no underline of a heading
            this.out += '- - -\n'
            return 0
        } else if(codeBlock){
            // the blank lines of a code block are kept (in a quote they are anyway, with the prefix)
            if(!('blockQuoteLevel' in blockFmt)) missedBlankCodeBlockLine = true
            if(!this.fencedCodeBlock){
                let fenceChar = blockFmt.codeFence || '`'
                this.codeBlockFence = fenceChar[0].repeat(3)
                if('indent' in blockFmt) this.codeBlockFence = ' '.repeat(this.wrappedLineIndent) + this.codeBlockFence
                this.out += this.codeBlockFence + (blockFmt.codeLanguage || '') + '\n' + this.linePrefix
                this.fencedCodeBlock = true
            }
            wrap = false
        } else if(!blockFmt.indent){
            this.wrappedLineIndent = 0
            if('codeLanguage' in blockFmt){
                // an indented code block (a quote can have one, not the other way round)
                this.linePrefix += '    '
                this.indentedCodeBlock = true
            }
            if(!this.linePrefixWritten){
                this.out += this.linePrefix
                this.linePrefixWritten = true
            }
        }
        if(blockFmt.headingLevel){
            let rawHeadingLevel = Number(blockFmt.headingLevel)
            let headingLevel = Number.isFinite(rawHeadingLevel) ? Math.floor(rawHeadingLevel) : 0
            headingLevel = Math.max(1, Math.min(6, headingLevel))
            if(headingLevel > 0){
                this.out += '#'.repeat(headingLevel) + ' '
                wrap = false
            }
        }
        let wrapIndentString = this.linePrefix + ' '.repeat(this.wrappedLineIndent)
        let col = wrapIndentString.length
        let mono = false, startsOrEndsWithBacktick = false, bold = false, italic = false, underline = false, strikeOut = false
        let endingMarkers = false
        let backticks = '`'
        for(let frag of doc.blockFragments(block)){
            missedBlankCodeBlockLine = false
            let fragmentText = frag.text.replace(/\n+$/, '')
            if(!(this.fencedCodeBlock || this.indentedCodeBlock)){
                fragmentText = escapeSpecialCharacters(fragmentText)
                fragmentText = maybeEscapeFirstChar(fragmentText)
            }
            if(list){
                fragmentText = fragmentText.split(LINE_SEPARATOR).join('\n' + ' '.repeat(this.wrappedLineIndent))
            } else if(blockFmt.indent > 0){
                this.out += ' '.repeat(this.wrappedLineIndent)
            } else {
                fragmentText = fragmentText.split(LINE_SEPARATOR).join('\n')
            }
            startsOrEndsWithBacktick = startsOrEndsWithBacktick || fragmentText.startsWith('`') || fragmentText.endsWith('`')
            let fmt = frag.format
            if(fmt.objectType === 'image'){
                let desc = fmt.imageAlt || 'image'
                let s = '![' + desc + '](' + (fmt.imageName || '')
                let title = fmt.imageTitle || ''
                if(title) s += ' "' + title + '"'
                s += ')'
                if(wrap && col + s.length > COLUMN_LIMIT){
                    this.out += '\n' + wrapIndentString
                    col = this.wrappedLineIndent
                }
                this.out += s
                col += s.length
            } else if('anchorHref' in fmt){
                let href = String(fmt.anchorHref)
                let hasToolTip = 'toolTip' in fmt
                let s
                if(!hasToolTip && href === fragmentText && hasScheme(href)){
                    s = '<' + href + '>'
                } else {
                    s = '[' + fragmentText + '](' + href
                    if(hasToolTip) s += ' ' + createLinkTitle(String(fmt.toolTip))
                    s += ')'
                }
                if(wrap && col + s.length > COLUMN_LIMIT){
                    this.out += '\n' + wrapIndentString
                    col = this.wrappedLineIndent
                }
                this.out += s
                col += s.length
            } else {
                let monoFrag = isMono(fmt)
                let fragBold = F.fontWeight(fmt) > 500
                let fragItalic = F.fontItalic(fmt)
                let fragStrikeOut = F.fontStrikeOut(fmt)
                let fragUnderline = F.fontUnderline(fmt)
                let markers = ''
                if(!ignoreFormat){
                    if(monoFrag !== mono && !this.indentedCodeBlock && !this.fencedCodeBlock){
                        if(monoFrag) backticks = '`'.repeat(adjacentBackticksCount(fragmentText) + 1)
                        markers += backticks
                        if(startsOrEndsWithBacktick) markers += ' '
                        mono = monoFrag
                        if(!mono) endingMarkers = true
                    }
                    if(!blockFmt.headingLevel && !mono){
                        if(fragBold !== bold){
                            markers += '**'
                            bold = fragBold
                            if(!bold) endingMarkers = true
                        }
                        if(fragItalic !== italic){
                            markers += '*'
                            italic = fragItalic
                            if(!italic) endingMarkers = true
                        }
                        if(fragStrikeOut !== strikeOut){
                            markers += '~~'
                            strikeOut = fragStrikeOut
                            if(!strikeOut) endingMarkers = true
                        }
                        if(fragUnderline !== underline){
                            // underline, which md4c tells from emphasis (GitHub dialect)
                            markers += '_'
                            underline = fragUnderline
                            if(!underline) endingMarkers = true
                        }
                    }
                }
                if(wrap && col + markers.length * 2 + fragmentText.length > COLUMN_LIMIT){
                    let i = 0
                    let fragLen = fragmentText.length
                    let breakingLine = false
                    while(i < fragLen){
                        if(col >= COLUMN_LIMIT){
                            this.out += markers + '\n' + wrapIndentString
                            markers = ''
                            col = this.wrappedLineIndent
                            while(i < fragLen && isSpace(fragmentText[i])) i++
                        }
                        let j = i + COLUMN_LIMIT - col
                        if(j < fragLen){
                            let wi = nearestWordWrapIndex(fragmentText, j)
                            if(wi < 0){
                                j = fragLen
                                // no break in the fragment: one before it
                                if(endingMarkers){
                                    this.out += markers
                                    markers = ''
                                }
                                this.out += '\n' + wrapIndentString
                                col = this.wrappedLineIndent
                            } else if(wi >= i){
                                j = wi
                                breakingLine = true
                            }
                        } else {
                            j = fragLen
                            breakingLine = false
                        }
                        let subfrag = fragmentText.slice(i, j)
                        if(!i){
                            this.out += markers
                            col += markers.length
                        }
                        if(col === this.wrappedLineIndent) subfrag = maybeEscapeFirstChar(subfrag)
                        this.out += subfrag
                        if(breakingLine){
                            this.out += '\n' + wrapIndentString
                            col = this.wrappedLineIndent
                        } else {
                            col += subfrag.length
                        }
                        i = j + 1
                    }
                } else {
                    if(!this.linePrefixWritten && col === wrapIndentString.length){
                        this.out += this.linePrefix
                        col += this.linePrefix.length
                    }
                    this.out += markers + fragmentText
                    col += markers.length + fragmentText.length
                }
            }
        }
        if(mono){
            if(startsOrEndsWithBacktick){
                this.out += ' '
                col++
            }
            this.out += backticks
            col += backticks.length
        }
        if(bold){
            this.out += '**'
            col += 2
        }
        if(italic){
            this.out += '*'
            col++
        }
        if(underline){
            this.out += '_'
            col++
        }
        if(strikeOut){
            this.out += '~~'
            col += 2
        }
        if(missedBlankCodeBlockLine) this.out += '\n'
        this.linePrefixWritten = false
        return col
    }
}

// QTextDocument::toMarkdown
function toMarkdown(doc){
    return new MarkdownWriter(doc).writeAll()
}

module.exports = { MarkdownImporter, MarkdownWriter, setMarkdown, toMarkdown, fragmentFromMarkdown, insertMarkdown, isMono, BEGINNING_OF_FRAME, END_OF_FRAME }
