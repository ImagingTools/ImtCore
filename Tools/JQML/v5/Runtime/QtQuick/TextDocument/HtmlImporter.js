// QTextHtmlImporter (qtextdocumentfragment.cpp of Qt 6.12): the nodes of the HTML parser into a document through a cursor
const F = require('./Formats')
const { Parser, isSpace, NONE, INLINE, WS_PRE, WS_PREWRAP, WS_PRELINE, WS_NOWRAP } = require('./HtmlParser')
const { TextCursor } = require('./Cursor')
const { LINE_SEPARATOR } = require('./Document')

const PRESERVE = 0, REMOVE = 1, COLLAPSE = 2

const IMPORT_TO_DOCUMENT = 0, IMPORT_TO_FRAGMENT = 1

// TableCellIterator: the cells of a table row by row, a spanned cell once
class CellIterator {
    constructor(doc, table){
        this.doc = doc
        this.table = table
        this.row = 0
        this.column = 0
    }

    grid(){
        let entry = this.doc.tables().find(e => e.table === this.table)
        return entry ? this.doc.tableGrid(entry) : null
    }

    atEnd(){
        let grid = this.grid()
        return !grid || this.row >= grid.rows
    }

    cell(){
        let grid = this.grid()
        return grid ? grid.cellAt(this.row, this.column) : null
    }

    next(){
        if(this.atEnd()) return
        let grid = this.grid()
        do {
            let cell = grid.cellAt(this.row, this.column)
            if(!cell) break
            this.column += cell.columnSpan
            if(this.column >= grid.columns){
                this.column = 0
                this.row++
            }
        } while(this.row < grid.rows && grid.cellAt(this.row, this.column) && grid.cellAt(this.row, this.column).row !== this.row)
    }
}

// nextListStyle: a nested <ul> goes disc, circle, square
function nextListStyle(style){
    if(style === F.LIST_DISC) return F.LIST_CIRCLE
    if(style === F.LIST_CIRCLE) return F.LIST_SQUARE
    return style
}

class HtmlImporter extends Parser {
    constructor(doc, html, mode){
        super()
        this.doc = doc
        this.importMode = mode
        this.cursor = new TextCursor(doc)
        this.indent = 0
        this.headingLevel = 0
        this.compressNextWhitespace = PRESERVE
        this.wsm = 'normal'
        this.lists = []
        this.tables = []
        this.namedAnchors = []
        let start = html.indexOf('<!--StartFragment-->')
        if(start !== -1){
            let header = '<meta name="qrichtext" content="1" />'
            let hasHeader = html.includes(header)
            let end = html.indexOf('<!--EndFragment-->')
            html = start < end ? html.slice(start, end) : html.slice(start)
            if(hasHeader) html = header + html
        }
        this.parse(html)
    }

    import(){
        this.hasBlock = true
        this.forceBlockMerging = false
        this.compressNextWhitespace = REMOVE
        this.blockTagClosed = false
        for(this.currentNodeIdx = 0; this.currentNodeIdx < this.count(); this.currentNodeIdx++){
            this.currentNode = this.at(this.currentNodeIdx)
            let node = this.currentNode
            this.wsm = this.textEditMode ? WS_PREWRAP : node.wsm
            // a closing tag passed: structures end, subsequent block tags collapse
            if(this.currentNodeIdx > 0 && node.parent !== this.currentNodeIdx - 1){
                let lastBlockTagClosed = this.closeTag()
                this.blockTagClosed = this.blockTagClosed || lastBlockTagClosed
                if(this.blockTagClosed && !node.isBlock() && node.id !== null){
                    this.hasBlock = false
                } else if(this.blockTagClosed && this.hasBlock){
                    let blockFormat = F.with_(node.blockFormat, { indent: this.indent })
                    let old = this.cursor.blockFormat()
                    if('pageBreakPolicy' in old){
                        let pageBreak = old.pageBreakPolicy
                        if(pageBreak === 0x2) pageBreak = 0x1
                        blockFormat = F.with_(blockFormat, { pageBreakPolicy: pageBreak })
                    }
                    this.cursor.setBlockFormat(blockFormat)
                }
            }
            if(node.displayMode === NONE){
                if(node.id === 'title') this.doc.title = node.text
                continue
            }
            if(this.processSpecialNodes() === 'next') continue
            // a block for 'Blah' after <ul><li>foo</ul>Blah
            if(this.blockTagClosed && !this.hasBlock && !node.isBlock() && node.text.length && !node.hasOnlyWhitespace() && node.displayMode === INLINE){
                let block = F.with_(node.blockFormat, { indent: this.indent })
                this.appendBlock(block, node.charFormat)
                this.blockTagClosed = false
                this.hasBlock = true
            }
            if(node.isBlock()){
                let result = this.processBlockNode()
                if(result === 'next') continue
                if(result === 'sibling'){
                    this.currentNodeIdx += node.children.length
                    continue
                }
            }
            if(node.charFormat.isAnchor){
                let names = node.charFormat.anchorNames
                if(names && names.length) this.namedAnchors.push(names[0])
            }
            if(this.appendNodeText()) this.hasBlock = false
        }
    }

    appendNodeText(){
        let initial = this.cursor.position
        let format = this.currentNode.charFormat
        let wsm = this.wsm
        if(wsm === WS_PRE || wsm === WS_PREWRAP) this.compressNextWhitespace = PRESERVE
        let text = this.currentNode.text
        let toInsert = ''
        for(let ch of text){
            if(isSpace(ch) && ch !== '\u00a0' && ch !== '\u2029'){
                if(wsm === WS_PRELINE && (ch === '\n' || ch === '\r')) this.compressNextWhitespace = PRESERVE
                if(this.compressNextWhitespace === COLLAPSE) this.compressNextWhitespace = REMOVE
                else if(this.compressNextWhitespace === REMOVE) continue
                if(wsm === WS_PRE || this.textEditMode){
                    if(ch === '\n'){
                        if(this.textEditMode) continue
                    } else if(ch === '\r') continue
                } else if(wsm !== WS_PREWRAP){
                    this.compressNextWhitespace = REMOVE
                    if(wsm === WS_PRELINE && (ch === '\n' || ch === '\r')){
                        // kept
                    } else if(wsm === WS_NOWRAP) ch = '\u00a0'
                    else ch = ' '
                }
            } else this.compressNextWhitespace = PRESERVE
            if(ch === '\n' || ch === '\u2029'){
                if(toInsert.length){
                    if(wsm === WS_PRELINE && toInsert.endsWith(' ')) toInsert = toInsert.slice(0, -1)
                    this.cursor.insertText(toInsert, format)
                    toInsert = ''
                }
                let fmt = this.cursor.blockFormat()
                if('bottomMargin' in fmt) this.cursor.setBlockFormat(F.without(fmt, 'bottomMargin'))
                fmt = F.without(fmt, 'topMargin')
                this.appendBlock(fmt, this.cursor.charFormat())
            } else if(this.namedAnchors.length){
                if(toInsert.length){
                    this.cursor.insertText(toInsert, format)
                    toInsert = ''
                }
                this.cursor.insertText(ch, F.with_(format, { isAnchor: true, anchorNames: this.namedAnchors.slice() }))
                this.namedAnchors = []
                format = F.without(format, 'isAnchor', 'anchorNames')
            } else toInsert += ch
        }
        if(toInsert.length) this.cursor.insertText(toInsert, format)
        return this.cursor.position !== initial
    }

    processSpecialNodes(){
        let node = this.currentNode
        switch(node.id){
            case 'body':
                if(node.charFormat.background){
                    this.doc.rootFrameFormat = F.with_(this.doc.rootFrameFormat, { background: node.charFormat.background })
                    node.charFormat = F.without(node.charFormat, 'background')
                }
                this.compressNextWhitespace = REMOVE
                break
            case 'ol': case 'ul': {
                let style = node.listStyle
                if(node.id === 'ul' && !node.hasOwnListStyle && node.parent){
                    for(let n = this.at(node.parent); n; n = n.parent ? this.at(n.parent) : null){
                        if(n.id === 'ul') style = nextListStyle(node.listStyle)
                    }
                }
                let listFormat = { style }
                if(node.textListNumberPrefix !== null) listFormat.numberPrefix = node.textListNumberPrefix
                if(node.textListNumberSuffix !== null) listFormat.numberSuffix = node.textListNumberSuffix
                if(node.listStart !== 1) listFormat.start = node.listStart
                if(node.hasCssListIndent){
                    this.indent += node.cssListIndent
                    listFormat.indent = node.cssListIndent
                } else {
                    this.indent++
                    listFormat.indent = this.indent
                }
                this.lists.push({ format: listFormat, list: null, listNode: this.currentNodeIdx })
                this.compressNextWhitespace = REMOVE
                // broken html: <ul>Text here<li>Foo
                let simplified = node.text.replace(/\s+/g, ' ').trim()
                if(!simplified.length) return 'next'
                break
            }
            case 'table':
                this.tables.push(this.scanTable(this.currentNodeIdx))
                this.hasBlock = false
                this.compressNextWhitespace = REMOVE
                return 'next'
            case 'tr':
                return 'next'
            case 'img': {
                let fmt = { objectType: 'image', imageName: node.imageName }
                if(node.text.length) fmt.imageTitle = node.text
                if(node.imageAlt.length) fmt.imageAlt = node.imageAlt
                fmt = F.merge(fmt, node.charFormat)
                if(node.imageWidth !== -1) fmt = F.with_(fmt, { imageWidth: node.imageWidth })
                if(node.imageHeight !== -1) fmt = F.with_(fmt, { imageHeight: node.imageHeight })
                this.cursor.insertImage(fmt)
                this.cursor.movePreviousCharacter(true)
                this.cursor.mergeCharFormat(node.charFormat)
                this.cursor.moveNextCharacter(false)
                this.compressNextWhitespace = COLLAPSE
                this.hasBlock = false
                return 'next'
            }
            case 'hr': {
                let blockFormat = F.with_(node.blockFormat, {
                    topMargin: this.topMargin(this.currentNodeIdx),
                    bottomMargin: this.bottomMargin(this.currentNodeIdx),
                    hrWidth: node.width || { type: 'variable', value: 0 },
                })
                if(this.hasBlock && this.importMode === IMPORT_TO_DOCUMENT) this.cursor.mergeBlockFormat(blockFormat)
                else this.appendBlock(blockFormat)
                this.hasBlock = false
                this.compressNextWhitespace = REMOVE
                return 'next'
            }
            case 'h1': this.headingLevel = 1; break
            case 'h2': this.headingLevel = 2; break
            case 'h3': this.headingLevel = 3; break
            case 'h4': this.headingLevel = 4; break
            case 'h5': this.headingLevel = 5; break
            case 'h6': this.headingLevel = 6; break
        }
        return 'current'
    }

    // whether a block tag was closed
    closeTag(){
        let closedNode = this.at(this.currentNodeIdx - 1)
        let endDepth = this.depth(this.currentNodeIdx) - 1
        let depth = this.depth(this.currentNodeIdx - 1)
        let blockTagClosed = false
        while(depth > endDepth){
            let t = this.tables.length ? this.tables[this.tables.length - 1] : null
            switch(closedNode.id){
                case 'tr':
                    if(t && t.table){
                        t.currentRow++
                        // broken html with row spans but no tr
                        while(!t.cells.atEnd() && t.cells.row < t.currentRow) t.cells.next()
                    }
                    blockTagClosed = true
                    break
                case 'table': {
                    if(!t) break
                    this.indent = t.lastIndent
                    this.tables.pop()
                    t = this.tables.length ? this.tables[this.tables.length - 1] : null
                    if(!t) this.cursor.setPosition(this.doc.length())
                    else if(t.table && !t.cells.atEnd()) this.cursor.setPosition(t.cells.cell().last)
                    // no extra block after a table
                    blockTagClosed = false
                    this.compressNextWhitespace = REMOVE
                    break
                }
                case 'th': case 'td':
                    if(t && t.table) t.cells.next()
                    blockTagClosed = true
                    this.compressNextWhitespace = REMOVE
                    break
                case 'ol': case 'ul':
                    if(!this.lists.length) break
                    this.lists.pop()
                    if(closedNode.hasCssListIndent) this.indent -= closedNode.cssListIndent
                    else this.indent--
                    blockTagClosed = true
                    break
                case 'br':
                    this.compressNextWhitespace = REMOVE
                    break
                case 'div':
                    if(this.cursor.position > 0){
                        let current = this.doc.characterAt(this.cursor.position - 1)
                        if(closedNode.children.length && current !== LINE_SEPARATOR) blockTagClosed = true
                    }
                    break
                case 'h1': case 'h2': case 'h3': case 'h4': case 'h5': case 'h6':
                    this.headingLevel = 0
                    blockTagClosed = true
                    break
                default:
                    if(closedNode.isBlock()) blockTagClosed = true
                    break
            }
            closedNode = this.at(closedNode.parent)
            depth--
        }
        return blockTagClosed
    }

    processBlockNode(){
        let node = this.currentNode
        let block = F.EMPTY
        let charFmt = F.EMPTY
        let modifiedBlockFormat = true
        let modifiedCharFormat = true
        if(node.isTableCell() && this.tables.length){
            let t = this.tables[this.tables.length - 1]
            if(t.table && !t.cells.atEnd()){
                let cell = t.cells.cell()
                if(cell){
                    // the cell's paddings and borders (QTextTableCellFormat)
                    let fmt = {}
                    let i = this.currentNodeIdx
                    if(this.topPadding(i) >= 0) fmt.topPadding = this.topPadding(i)
                    if(this.bottomPadding(i) >= 0) fmt.bottomPadding = this.bottomPadding(i)
                    if(this.leftPadding(i) >= 0) fmt.leftPadding = this.leftPadding(i)
                    if(this.rightPadding(i) >= 0) fmt.rightPadding = this.rightPadding(i)
                    let names = ['top', 'right', 'bottom', 'left']
                    for(let e = 0; e < 4; e++){
                        if(node.tableCellBorder[e] > 0) fmt[names[e] + 'Border'] = node.tableCellBorder[e]
                    }
                    for(let e = 0; e < 4; e++){
                        if(node.tableCellBorderStyle[e] !== 'none') fmt[names[e] + 'BorderStyle'] = node.tableCellBorderStyle[e]
                    }
                    for(let e = 0; e < 4; e++){
                        if(node.tableCellBorderBrush[e]) fmt[names[e] + 'BorderBrush'] = node.tableCellBorderBrush[e]
                    }
                    let old = this.doc.charFormatAt(cell.marker)
                    this.doc.setCellFormat(cell.marker, F.merge(old, fmt))
                    this.cursor.setPosition(cell.first)
                }
            }
            this.hasBlock = true
            this.compressNextWhitespace = REMOVE
            if(node.charFormat.background){
                charFmt = F.with_(charFmt, { background: node.charFormat.background })
                this.cursor.mergeBlockCharFormat(charFmt)
            }
        }
        if(this.hasBlock){
            block = this.cursor.blockFormat()
            charFmt = this.cursor.blockCharFormat()
            modifiedBlockFormat = false
            modifiedCharFormat = false
        }
        // collapse
        let tm = this.topMargin(this.currentNodeIdx)
        if(tm > (block.topMargin || 0)){
            block = F.with_(block, { topMargin: tm })
            modifiedBlockFormat = true
        }
        let bottomMargin = this.bottomMargin(this.currentNodeIdx)
        // a list item collapses with the bottom margin of its list
        let parentNode = node.parent ? this.at(node.parent) : null
        if((node.id === 'li' || node.id === 'dt' || node.id === 'dd') && parentNode
            && (parentNode.isListStart() || parentNode.id === 'dl')
            && parentNode.children[parentNode.children.length - 1] === this.currentNodeIdx){
            bottomMargin = Math.max(bottomMargin, this.bottomMargin(node.parent))
        }
        if((block.bottomMargin || 0) !== bottomMargin){
            block = F.with_(block, { bottomMargin })
            modifiedBlockFormat = true
        }
        let lm = this.leftMargin(this.currentNodeIdx), rm = this.rightMargin(this.currentNodeIdx)
        if((block.leftMargin || 0) !== lm){
            block = F.with_(block, { leftMargin: lm })
            modifiedBlockFormat = true
        }
        if((block.rightMargin || 0) !== rm){
            block = F.with_(block, { rightMargin: rm })
            modifiedBlockFormat = true
        }
        if(node.id !== 'li' && this.indent !== 0
            && (!this.lists.length || !this.hasBlock || !this.lists[this.lists.length - 1].list
                || this.doc.itemNumber(this.lists[this.lists.length - 1].list, this.cursor.block()) === -1)){
            block = F.with_(block, { indent: this.indent })
            modifiedBlockFormat = true
        }
        if(this.headingLevel){
            block = F.with_(block, { headingLevel: this.headingLevel })
            modifiedBlockFormat = true
        }
        if(F.count(node.blockFormat) > 0){
            modifiedBlockFormat = true
            block = F.merge(block, node.blockFormat)
        }
        if(F.count(node.charFormat) > 0){
            modifiedCharFormat = true
            charFmt = F.merge(charFmt, node.charFormat)
        }
        if(this.wsm === WS_PRE || this.wsm === WS_NOWRAP){
            block = F.with_(block, { nonBreakableLines: true })
            modifiedBlockFormat = true
        }
        if(node.charFormat.background && !node.isTableCell()){
            block = F.with_(block, { background: node.charFormat.background })
            modifiedBlockFormat = true
        }
        if(this.hasBlock && (!node.isEmptyParagraph || this.forceBlockMerging)){
            if(modifiedBlockFormat) this.cursor.setBlockFormat(block)
            if(modifiedCharFormat) this.cursor.setBlockCharFormat(charFmt)
        } else {
            if(this.currentNodeIdx === 1 && this.cursor.position === 0 && node.isEmptyParagraph){
                this.cursor.setBlockFormat(block)
                this.cursor.setBlockCharFormat(charFmt)
            } else this.appendBlock(block, charFmt)
        }
        if(node.userState !== -1) this.doc.blocks[this.cursor.block()].userState = node.userState
        if(node.id === 'li' && this.lists.length){
            let l = this.lists[this.lists.length - 1]
            if(l.list){
                this.cursor.mergeBlockFormat({ list: l.list })
            } else {
                l.list = this.cursor.createList(l.format)
                let listTopMargin = this.topMargin(l.listNode)
                if(listTopMargin > (block.topMargin || 0)){
                    block = F.with_(block, { topMargin: listTopMargin })
                    this.cursor.mergeBlockFormat(block)
                }
            }
            if(this.hasBlock){
                let fmt = {}
                fmt.indent = node.blockFormat.indent || 0
                this.cursor.mergeBlockFormat(fmt)
            }
        }
        this.forceBlockMerging = node.id === 'body' || node.id === 'html'
        if(node.isEmptyParagraph){
            this.hasBlock = false
            return 'sibling'
        }
        this.hasBlock = true
        this.blockTagClosed = false
        return 'current'
    }

    // QTextHtmlImporter::scanTable: the rows and columns of a table (spans and thead / tbody / tfoot rows), the table
    // inserted at the cursor with its cells merged; the cursor stays before it
    scanTable(tableNodeIdx){
        let table = { table: null, isTextFrame: false, rows: 0, columns: 0, currentRow: 0, cells: null, lastIndent: 0 }
        let columnWidths = []
        let headerRowCount = 0
        let rowNodes = []
        for(let row of this.at(tableNodeIdx).children){
            let id = this.at(row).id
            if(id === 'tr') rowNodes.push(row)
            else if(id === 'thead' || id === 'tbody' || id === 'tfoot'){
                for(let potential of this.at(row).children){
                    if(this.at(potential).id === 'tr'){
                        rowNodes.push(potential)
                        if(id === 'thead') headerRowCount++
                    }
                }
            }
        }
        let spans = []
        let spanForColumn = []
        let effectiveRow = 0
        for(let row of rowNodes){
            let colsInRow = 0
            for(let cellIdx of this.at(row).children){
                let c = this.at(cellIdx)
                if(!c.isTableCell()) continue
                // the columns spanned by the rows above
                while(colsInRow < spanForColumn.length){
                    let span = spanForColumn[colsInRow]
                    if(span && span.row + span.rowSpan > effectiveRow) colsInRow += span.colSpan
                    else break
                }
                let currentColumn = colsInRow
                colsInRow += c.tableCellColSpan
                let span = { row: effectiveRow, col: currentColumn, colSpan: c.tableCellColSpan, rowSpan: c.tableCellRowSpan }
                if(span.colSpan > 1 || span.rowSpan > 1) spans.push(span)
                while(columnWidths.length < colsInRow) columnWidths.push({ type: 'variable', value: 0 })
                while(spanForColumn.length < columnWidths.length) spanForColumn.push(null)
                for(let i = currentColumn; i < currentColumn + c.tableCellColSpan; i++){
                    if(columnWidths[i].type === 'variable' && c.width){
                        let w = c.width
                        if(c.tableCellColSpan > 1) w = { type: w.type, value: w.value / c.tableCellColSpan }
                        columnWidths[i] = w
                    }
                    spanForColumn[i] = span
                }
            }
            table.columns = Math.max(table.columns, colsInRow)
            effectiveRow++
        }
        table.rows = effectiveRow
        table.lastIndent = this.indent
        this.indent = 0
        if(table.rows === 0 || table.columns === 0) return table
        let node = this.at(tableNodeIdx)
        let fmt = {
            cellSpacing: node.tableCellSpacing,
            cellPadding: node.tableCellPadding,
            columnWidthConstraints: columnWidths,
            headerRowCount,
            borderCollapse: node.borderCollapse,
        }
        if('alignment' in node.blockFormat) fmt.alignment = node.blockFormat.alignment
        fmt.topMargin = this.topMargin(tableNodeIdx)
        fmt.bottomMargin = this.bottomMargin(tableNodeIdx)
        fmt.leftMargin = this.leftMargin(tableNodeIdx) + table.lastIndent * 40
        fmt.rightMargin = this.rightMargin(tableNodeIdx)
        if(fmt.leftMargin === fmt.rightMargin && fmt.leftMargin === fmt.topMargin && fmt.leftMargin === fmt.bottomMargin) fmt.margin = fmt.leftMargin
        fmt.borderStyle = node.borderStyle
        fmt.borderBrush = node.borderBrush
        fmt.border = node.tableBorder
        if(node.width) fmt.width = node.width
        if(node.height) fmt.height = node.height
        if('pageBreakPolicy' in node.blockFormat) fmt.pageBreakPolicy = node.blockFormat.pageBreakPolicy
        if('layoutDirection' in node.blockFormat) fmt.layoutDirection = node.blockFormat.layoutDirection
        if(node.charFormat.background) fmt.background = node.charFormat.background
        if(node.isTextFrame){
            // a frame of its own (-qt-table-type: frame) is laid out as its content
            table.isTextFrame = true
            return table
        }
        let oldPos = this.cursor.position
        table.table = this.cursor.insertTable(table.rows, table.columns, fmt)
        for(let span of spans) this.doc.mergeCells(table.table, span.row, span.col, span.rowSpan, span.colSpan)
        table.cells = new CellIterator(this.doc, table.table)
        this.cursor.setPosition(oldPos)
        return table
    }

    appendBlock(format, charFmt = F.EMPTY){
        if(this.namedAnchors.length){
            charFmt = F.with_(charFmt, { isAnchor: true, anchorNames: this.namedAnchors.slice() })
            this.namedAnchors = []
        }
        this.cursor.insertBlock(format, charFmt)
        if(this.wsm !== WS_PRE && this.wsm !== WS_PREWRAP) this.compressNextWhitespace = REMOVE
    }
}

// QTextDocument::setHtml
function setHtml(doc, html){
    doc.clear()
    new HtmlImporter(doc, html, IMPORT_TO_DOCUMENT).import()
}

module.exports = { HtmlImporter, setHtml, IMPORT_TO_DOCUMENT, IMPORT_TO_FRAGMENT }
