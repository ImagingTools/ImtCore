// The layout of a document (QTextDocumentLayout and QTextLine of Qt 6.12, without floats and pages): blocks one under
// the other with the larger of the bottom margin before and the top margin as the space between them, the lines of a
// block broken at its width, each line as high as its tallest font, a list item's marker left of its first line.
const F = require('./Formats')
const { OBJECT_REPLACEMENT, LINE_SEPARATOR } = require('./Document')

// QTextDocument::indentWidth
const INDENT_WIDTH = 40

// limit table row allocations to avoid resource exhaustion on hostile/malformed content
const MAX_TABLE_LAYOUT_ROWS = 10000

// wrap modes (QTextOption::WrapMode as TextEdit.wrapMode has them)
const NO_WRAP = 0, WORD_WRAP = 1, WRAP_ANYWHERE = 3, WRAP = 4

// FontSizeAdjustment of HTML sizes (QTextFormatPrivate::resolveFont)
const SIZE_FACTORS = [0.7, 0.8, 1.0, 1.2, 1.5, 2, 2.4]

const PX_PER_PT = 96 / 72

// the font of a char format over the document's default font (QTextCharFormat::font resolved on the default font)
function resolveFont(format, base){
    let pixelSize
    if('sizeAdjustment' in format){
        let factor = SIZE_FACTORS[Math.max(0, Math.min(6, format.sizeAdjustment + 2))]
        pixelSize = base.sizeUnit === 'px' ? Math.round(factor * base.pixelSize) : factor * base.pointSize * PX_PER_PT
    } else if('pointSize' in format) pixelSize = format.pointSize * PX_PER_PT
    else if('pixelSize' in format) pixelSize = format.pixelSize
    else pixelSize = base.sizeUnit === 'px' ? base.pixelSize : base.pointSize * PX_PER_PT
    let valign = F.verticalAlignment(format)
    // QTextEngine: sub- and superscript in two thirds of the size
    if(valign === F.ALIGN_SUBSCRIPT || valign === F.ALIGN_SUPERSCRIPT) pixelSize = Math.trunc(pixelSize * 2 / 3)
    let letterSpacing = base.letterSpacing || 0
    if('letterSpacing' in format) letterSpacing = format.letterSpacingType === 'percent' ? pixelSize * (format.letterSpacing / 100 - 1) : format.letterSpacing
    return {
        family: format.families && format.families.length ? format.families.join(', ') : base.family,
        pixelSize,
        weight: 'weight' in format ? format.weight : base.weight,
        italic: 'italic' in format ? !!format.italic : !!base.italic,
        underline: ('underlineStyle' in format || 'fontUnderline' in format) ? F.fontUnderline(format) : !!base.underline,
        overline: 'overline' in format ? !!format.overline : false,
        strikeout: 'strikeOut' in format ? !!format.strikeOut : !!base.strikeout,
        capitalization: 'capitalization' in format ? format.capitalization : (base.capitalization || 0),
        letterSpacing,
        wordSpacing: 'wordSpacing' in format ? format.wordSpacing : (base.wordSpacing || 0),
        valign,
    }
}

// the text as the capitalization shows it
function displayText(text, font){
    if(font.capitalization === 1 || font.capitalization === 3) return text.toUpperCase()
    if(font.capitalization === 2) return text.toLowerCase()
    if(font.capitalization === 4) return text.replace(/(^|\s)(\S)/g, (m, s, c) => s + c.toUpperCase())
    return text
}

// QTextList::itemText
function itemText(list, number){
    let format = list.format
    let n = number + ('start' in format ? format.start : 1) - 1
    let prefix = 'numberPrefix' in format ? format.numberPrefix : ''
    let suffix = 'numberSuffix' in format ? format.numberSuffix : '.'
    let result = ''
    switch(format.style){
        case F.LIST_DECIMAL: result = String(n); break
        case F.LIST_LOWER_ALPHA: case F.LIST_UPPER_ALPHA: {
            if(n < 1){
                result = String(n)
                break
            }
            let base = format.style === F.LIST_UPPER_ALPHA ? 65 : 97
            for(let c = n; c > 0;){
                c--
                result = String.fromCharCode(base + (c % 26)) + result
                c = Math.trunc(c / 26)
            }
            break
        }
        case F.LIST_LOWER_ROMAN: case F.LIST_UPPER_ROMAN: {
            if(n < 1 || n >= 5000){
                result = String(n)
                break
            }
            let numerals = [[1000, 'm'], [900, 'cm'], [500, 'd'], [400, 'cd'], [100, 'c'], [90, 'xc'], [50, 'l'], [40, 'xl'], [10, 'x'], [9, 'ix'], [5, 'v'], [4, 'iv'], [1, 'i']]
            let v = n
            for(let [value, symbol] of numerals) while(v >= value){ result += symbol; v -= value }
            if(format.style === F.LIST_UPPER_ROMAN) result = result.toUpperCase()
            break
        }
        default: return ''
    }
    return prefix + result + suffix
}

// the fonts of the formats over a default font, kept while the formats live (a format is not changed once made); each
// font keeps the widths of the texts measured in it
const fontsOfFormat = new WeakMap()
const formatIds = new WeakMap()
let nextFormatId = 1

// a number for a format object (the layout of a block is kept by the formats of its pieces)
function formatId(format){
    let id = formatIds.get(format)
    if(!id){
        id = nextFormatId++
        formatIds.set(format, id)
    }
    return id
}

class Layouter {
    // options: { font (the default font: family, pixelSize, pointSize, sizeUnit, weight, italic...), width (the text
    // width to wrap at, Infinity for none), wrapMode, alignment (Qt::Alignment of the text option), margin (the document
    // margin), tabStop, measure(text, font) -> { advance, height, line, ascent }, imageSize(name) -> { width, height },
    // blockCache (a Map the layouts of blocks are kept in from one layout to the next) }
    constructor(doc, options){
        this.doc = doc
        this.o = options
        let base = options.font
        this.baseKey = [base.family, base.pixelSize, base.pointSize, base.sizeUnit, base.weight, base.italic, base.underline, base.strikeout,
            base.capitalization, base.letterSpacing, base.wordSpacing].join('|')
    }

    font(format){
        let fonts = fontsOfFormat.get(format)
        if(!fonts){
            fonts = new Map()
            fontsOfFormat.set(format, fonts)
        }
        let font = fonts.get(this.baseKey)
        if(!font){
            font = resolveFont(format, this.o.font)
            Object.defineProperty(font, '$widths', { value: new Map() })
            fonts.set(this.baseKey, font)
        }
        return font
    }

    metrics(font){
        if(font.$metrics) return font.$metrics
        let m = this.o.measure('', font)
        let metrics = { ascent: m.ascent, descent: Math.max(0, m.line - m.ascent) }
        if(font.$widths) Object.defineProperty(font, '$metrics', { value: metrics })
        return metrics
    }

    width(text, font){
        if(!text) return 0
        let widths = font.$widths
        if(widths){
            let w = widths.get(text)
            if(w !== undefined) return w
        }
        let shown = displayText(text, font)
        let w = this.o.measure(shown, font).advance
        if(font.wordSpacing){
            let spaces = (shown.match(/ /g) || []).length
            w += spaces * font.wordSpacing
        }
        if(widths){
            if(widths.size > 5000) widths.clear()
            widths.set(text, w)
        }
        return w
    }

    // the number of a list item among its list's items (QTextList::itemNumber), the lists numbered once a layout
    itemNumber(list, index){
        if(!this.listNumbers){
            this.listNumbers = new Map()
            let counts = new Map()
            let blocks = this.doc.blocks
            for(let i = 0; i < blocks.length; i++){
                let l = blocks[i].format.list
                if(!l) continue
                let n = counts.get(l) || 0
                this.listNumbers.set(i, n)
                counts.set(l, n + 1)
            }
        }
        let n = this.listNumbers.get(index)
        return n === undefined ? -1 : n
    }

    // the layout of a block kept from a layout before when nothing it depends on changed: its pieces and their formats,
    // its formats, the width, its list number; moved to where the block is now
    cachedBlock(index, x0, y, textWidth, alignWidth){
        let cache = this.o.blockCache
        if(!cache) return this.layoutBlock(index, x0, y, textWidth, alignWidth)
        let doc = this.doc
        let format = doc.blockFormat(index)
        let list = format.list
        let key = doc.blocks[index].sig + '/' + formatId(format) + '/' + formatId(doc.blockCharFormat(index)) + '/' + textWidth + '/' + alignWidth + '/' + this.o.alignment + '/'
            + this.o.wrapMode + '/' + this.baseKey + '/' + (list ? this.itemNumber(list, index) : '') + '/' + (this.o.imageVersion || 0)
        let start = doc.blockStart(index)
        let kept = cache.get(key)
        if(!kept){
            kept = this.layoutBlock(index, 0, 0, textWidth, alignWidth)
            relocate(kept, 0, 0, -start)
            if(cache.size > 20000) cache.clear()
            cache.set(key, kept)
        }
        // the block placed where it was the last time: the same layout
        if(kept.placed && kept.px === x0 && kept.py === y && kept.pstart === start && kept.placed.index === index) return kept.placed
        let block = copyBlock(kept, index, format)
        relocate(block, x0, y, start)
        kept.placed = block
        kept.px = x0
        kept.py = y
        kept.pstart = start
        return block
    }

    // the pieces of a block: characters of one format split into words, spaces, tabs, line separators and objects
    tokens(index){
        let tokens = []
        for(let fragment of this.doc.blockFragments(index)){
            let font = this.font(fragment.format)
            let text = fragment.text
            let i = 0
            while(i < text.length){
                let c = text[i]
                let start = fragment.start + i
                let kind, end = i + 1
                if(c === LINE_SEPARATOR) kind = 'break'
                else if(c === '\t') kind = 'tab'
                else if(c === OBJECT_REPLACEMENT) kind = 'object'
                else if(c === ' '){
                    kind = 'space'
                    while(end < text.length && text[end] === ' ') end++
                } else {
                    kind = 'word'
                    while(end < text.length && !/[ \t\ufffc\u2028]/.test(text[end])) end++
                }
                let piece = text.slice(i, end)
                let token = { kind, text: piece, start, end: fragment.start + end, format: fragment.format, font, width: 0 }
                if(kind === 'word' || kind === 'space') token.width = this.width(piece, font)
                else if(kind === 'object'){
                    let size = this.imageSize(fragment.format)
                    token.width = size.width
                    token.height = size.height
                }
                tokens.push(token)
                i = end
            }
        }
        // a word goes on over the formats: its pieces are glued
        for(let i = 1; i < tokens.length; i++){
            if(tokens[i].kind === 'word' && tokens[i - 1].kind === 'word') tokens[i].glued = true
        }
        return tokens
    }

    imageSize(format){
        let width = 'imageWidth' in format ? format.imageWidth : -1
        let height = 'imageHeight' in format ? format.imageHeight : -1
        if(width < 0 || height < 0){
            let natural = this.o.imageSize ? this.o.imageSize(format.imageName || '') : null
            if(!natural) natural = { width: 16, height: 16 }
            if(width < 0 && height < 0){
                width = natural.width
                height = natural.height
            } else if(width < 0) width = natural.height ? natural.width * height / natural.height : natural.width
            else height = natural.width ? natural.height * width / natural.width : natural.height
        }
        return { width, height }
    }

    // a word too wide for the line split at the characters that fit (at least one)
    splitWord(token, room){
        let n = 1
        while(n < token.text.length && this.width(token.text.slice(0, n + 1), token.font) <= room) n++
        let head = Object.assign({}, token, { text: token.text.slice(0, n), end: token.start + n })
        head.width = this.width(head.text, token.font)
        let tail = Object.assign({}, token, { text: token.text.slice(n), start: token.start + n, glued: false })
        tail.width = this.width(tail.text, token.font)
        return [head, tail]
    }

    // the lines of a block: greedy breaking at the width (QTextLine::layout_helper with the wrap mode)
    breakLines(tokens, available, firstIndent, wrap){
        let lines = []
        let line = { items: [], width: 0 }
        let indent = firstIndent
        let room = () => available - indent
        let finish = (forced) => {
            line.forced = forced
            lines.push(line)
            line = { items: [], width: 0 }
            indent = 0
        }
        let tabWidth = (x) => {
            let stop = this.o.tabStop > 0 ? this.o.tabStop : 80
            let next = (Math.floor((x + indent) / stop) + 1) * stop - indent
            return Math.max(1, next - x)
        }
        let queue = tokens.slice()
        while(queue.length){
            let token = queue.shift()
            if(token.kind === 'break'){
                line.items.push(Object.assign({}, token, { width: 0 }))
                finish(true)
                continue
            }
            if(token.kind === 'tab') token = Object.assign({}, token, { width: tabWidth(line.width) })
            if(token.kind === 'space'){
                line.items.push(token)
                line.width += token.width
                continue
            }
            // the word with the pieces glued to it
            let word = [token]
            while(queue.length && queue[0].glued) word.push(queue.shift())
            let wordWidth = word.reduce((w, t) => w + t.width, 0)
            let fits = wrap === NO_WRAP || available === Infinity || line.width + wordWidth <= room() + 0.001
            if(fits){
                for(let t of word){
                    line.items.push(t)
                    line.width += t.width
                }
                continue
            }
            let hasContent = line.items.some(t => t.kind !== 'space')
            if(wrap === WRAP_ANYWHERE || ((wrap === WRAP || !hasContent) && wrap !== WORD_WRAP && wordWidth > room())){
                // the characters that fit on this line, the rest on the next
                let pieces = word.slice()
                while(pieces.length){
                    let t = pieces[0]
                    let left = room() - line.width
                    if(t.width <= left + 0.001 || t.kind === 'object'){
                        if(t.width > left + 0.001 && line.items.length) { finish(false); continue }
                        line.items.push(t)
                        line.width += t.width
                        pieces.shift()
                        continue
                    }
                    if(left <= 0 && line.items.length){
                        finish(false)
                        continue
                    }
                    let [head, tail] = this.splitWord(t, Math.max(0, left))
                    if(head.width > left + 0.001 && line.items.length){
                        finish(false)
                        continue
                    }
                    line.items.push(head)
                    line.width += head.width
                    pieces[0] = tail
                    finish(false)
                }
                continue
            }
            if(hasContent || line.items.length){
                if(hasContent) finish(false)
            }
            for(let t of word){
                line.items.push(t)
                line.width += t.width
            }
        }
        if(line.items.length || !lines.length || lines[lines.length - 1].forced) finish(false)
        return lines
    }

    // the document: its root frame from the document margin (QTextDocumentLayoutPrivate::layoutFrame)
    layout(){
        let doc = this.doc
        let o = this.o
        let margin = o.margin || 0
        this.tableList = doc.tables().map(entry => Object.assign(entry, {
            firstBlock: doc.blockIndexAt(entry.start + 1),
            lastBlock: doc.blockIndexAt(entry.end),
        }))
        let textWidth = o.width === Infinity ? Infinity : Math.max(0, o.width - 2 * margin)
        let alignWidth = o.alignWidth > 0 ? Math.max(0, o.alignWidth - 2 * margin) : 0
        let flow = this.layoutFlow(0, doc.blocks.length, margin, margin, textWidth, alignWidth, [])
        return { blocks: flow.blocks, tables: flow.tables, width: flow.width, height: flow.y + margin, lineCount: flow.lineCount, margin }
    }

    // a table starts at a block when its first U+FDD0 ends the block before
    tableAt(i, ancestors){
        return this.tableList.find(t => t.firstBlock === i && t.start === this.doc.blockStart(i) - 1 && !ancestors.includes(t))
    }

    // the blocks [from, to) and the tables among them one under the other from y at x, in the width (Infinity: none)
    // (QTextDocumentLayoutPrivate::layoutFlow); an empty block right before or after a table takes no room
    layoutFlow(from, to, x0, y, textWidth, alignWidth, ancestors){
        let doc = this.doc
        let blocks = [], tables = []
        let width = 0, lineCount = 0
        let previous = null
        for(let index = from; index < to; index++){
            let entry = this.tableAt(index, ancestors)
            if(entry){
                let format = entry.table.format
                if(previous) y += Math.max(format.topMargin || 0, previous.bottomMargin || 0)
                let table = this.layoutTable(entry, x0, y, textWidth, alignWidth, ancestors.concat([entry]))
                tables.push(table)
                for(let b of table.blocks) blocks.push(b)
                for(let t of table.tables) tables.push(t)
                lineCount += table.lineCount
                width = Math.max(width, table.x - x0 + table.width)
                y = table.y + table.height
                previous = { bottomMargin: format.bottomMargin || 0 }
                index = entry.lastBlock
                continue
            }
            let format = doc.blockFormat(index)
            let nextTable = index + 1 < to ? this.tableAt(index + 1, ancestors) : null
            let afterTable = index > from && doc.separatorBefore(index) === '\ufdd1' && this.tableList.some(t => t.end === doc.blockStart(index) - 1)
            let empty = doc.blockLength(index) === 1
            let origY = y
            if(previous) y += Math.max(format.topMargin || 0, previous.bottomMargin || 0)
            let block = this.cachedBlock(index, x0, y, textWidth, alignWidth)
            // a kept layout is not changed: a copy is moved
            if(empty && (nextTable || afterTable)) block = copyBlock(block, block.index, block.format)
            if(empty && nextTable){
                // QTextDocumentLayoutPrivate::isEmptyBlockBeforeTable: hidden in the table's border
                y = origY + Math.max(previous ? previous.bottomMargin || 0 : 0, format.topMargin || 0)
                for(let line of block.lines) line.top = y - line.lineHeight
                block.bottom = y
            } else if(empty && afterTable){
                // isEmptyBlockAfterTable: at the table's bottom, no room of its own
                for(let line of block.lines) line.top = y - line.lineHeight
                block.top = block.bottom = y
                y = origY
            } else y = block.bottom
            blocks.push(block)
            lineCount += block.lines.length
            width = Math.max(width, block.width)
            previous = format
        }
        return { blocks, tables, y, width, lineCount }
    }

    // QTextDocumentLayoutPrivate::layoutTable: the columns as wide as their widest cell (fixed and percentage widths
    // as the table's format sets them), shrunk to the width there is; the rows as high as their highest cell; cell
    // spacing between the cells, the border around the table and its cells, the padding in each cell
    layoutTable(entry, x0, y, textWidth, alignWidth, ancestors){
        let doc = this.doc
        let format = entry.table.format
        let grid = doc.tableGrid(entry)
        let spacing = 'cellSpacing' in format ? format.cellSpacing : 2
        let padding = 'cellPadding' in format ? format.cellPadding : 0
        let border = format.border || 0
        let left = x0 + (format.leftMargin || 0)
        let available = textWidth === Infinity ? Infinity : Math.max(0, textWidth - (format.leftMargin || 0) - (format.rightMargin || 0))
        let pad = (cell, side) => (side + 'Padding') in cell.format ? cell.format[side + 'Padding'] : padding
        let cellBorder = border > 0 ? 1 : 0
        // the natural width of each cell: its content unwrapped
        let natural = new Map()
        for(let cell of grid.cells){
            let fromBlock = doc.blockIndexAt(cell.first), toBlock = doc.blockIndexAt(cell.last) + 1
            let flow = this.layoutFlow(fromBlock, toBlock, 0, 0, Infinity, 0, ancestors)
            natural.set(cell, flow.width + pad(cell, 'left') + pad(cell, 'right') + 2 * cellBorder)
        }
        let columns = grid.columns
        let widths = new Array(columns).fill(0)
        for(let cell of grid.cells) if(cell.columnSpan === 1) widths[cell.column] = Math.max(widths[cell.column], natural.get(cell))
        for(let cell of grid.cells){
            if(cell.columnSpan === 1) continue
            let have = 0
            for(let c = cell.column; c < cell.column + cell.columnSpan; c++) have += widths[c]
            have += spacing * (cell.columnSpan - 1)
            let need = natural.get(cell) - have
            if(need > 0) for(let c = cell.column; c < cell.column + cell.columnSpan; c++) widths[c] += need / cell.columnSpan
        }
        let chrome = 2 * border + spacing * (columns + 1)
        let constraints = format.columnWidthConstraints || []
        let tableWidth = null
        if(format.width && format.width.type === 'fixed') tableWidth = format.width.value
        else if(format.width && format.width.type === 'percent' && available !== Infinity) tableWidth = available * format.width.value / 100
        let inner = tableWidth !== null ? Math.max(0, tableWidth - chrome) : null
        for(let c = 0; c < columns; c++){
            let constraint = constraints[c]
            if(!constraint || constraint.type === 'variable') continue
            if(constraint.type === 'fixed') widths[c] = Math.max(widths[c], constraint.value)
            else if(constraint.type === 'percent' && (inner !== null || available !== Infinity)) widths[c] = Math.max(widths[c], (inner !== null ? inner : available - chrome) * constraint.value / 100)
        }
        let sum = widths.reduce((a, b) => a + b, 0)
        if(inner !== null && sum < inner){
            // the room left goes to the columns without a width of their own
            let free = widths.map((w, c) => !constraints[c] || constraints[c].type === 'variable')
            let count = free.filter(Boolean).length || columns
            let extra = (inner - sum) / count
            widths = widths.map((w, c) => free[c] || count === columns ? w + extra : w)
        } else if(available !== Infinity && sum + chrome > available){
            // shrunk to the width there is, the content wraps
            let target = Math.max(0, available - chrome)
            let scale = sum > 0 ? target / sum : 0
            widths = widths.map(w => w * scale)
        }
        let width = widths.reduce((a, b) => a + b, 0) + chrome
        let x = left
        let align = format.alignment || 0
        let room = available !== Infinity ? available : (alignWidth > 0 ? alignWidth : width)
        if(room > width){
            if(align & 0x2) x += room - width
            else if(align & 0x4) x += (room - width) / 2
        }
        let columnX = []
        let cx = x + border + spacing
        for(let c = 0; c < columns; c++){
            columnX.push(cx)
            cx += widths[c] + spacing
        }
        // the cells laid out in their width; the rows as high as their cells
        let laid = []
        let rowCount = Number.isFinite(grid.rows) ? Math.max(0, Math.floor(grid.rows)) : 0
        rowCount = Math.min(rowCount, MAX_TABLE_LAYOUT_ROWS)
        let rowHeights = new Array(rowCount).fill(0)
        for(let cell of grid.cells){
            let cellWidth = widths.slice(cell.column, cell.column + cell.columnSpan).reduce((a, b) => a + b, 0) + spacing * (cell.columnSpan - 1)
            let contentWidth = Math.max(0, cellWidth - pad(cell, 'left') - pad(cell, 'right') - 2 * cellBorder)
            laid.push({ cell, cellWidth, contentWidth })
        }
        let blocks = [], tables = [], lineCount = 0
        let heights = new Map()
        for(let item of laid){
            let fromBlock = doc.blockIndexAt(item.cell.first), toBlock = doc.blockIndexAt(item.cell.last) + 1
            let flow = this.layoutFlow(fromBlock, toBlock, 0, 0, textWidth === Infinity && !(this.o.wrapMode) ? Infinity : item.contentWidth, item.contentWidth, ancestors)
            item.flow = flow
            let h = flow.y + pad(item.cell, 'top') + pad(item.cell, 'bottom') + 2 * cellBorder
            heights.set(item.cell, h)
            if(item.cell.rowSpan === 1) rowHeights[item.cell.row] = Math.max(rowHeights[item.cell.row], h)
        }
        for(let item of laid){
            let cell = item.cell
            if(cell.rowSpan === 1) continue
            let have = rowHeights.slice(cell.row, cell.row + cell.rowSpan).reduce((a, b) => a + b, 0) + spacing * (cell.rowSpan - 1)
            let need = heights.get(cell) - have
            if(need > 0) rowHeights[cell.row + cell.rowSpan - 1] += need
        }
        let rowY = []
        let ry = y + border + spacing
        for(let r = 0; r < grid.rows; r++){
            rowY.push(ry)
            ry += rowHeights[r] + spacing
        }
        let height = ry - y + border - spacing + spacing
        let cells = []
        for(let item of laid){
            let cell = item.cell
            let cellX = columnX[cell.column]
            let cellY = rowY[cell.row]
            let cellHeight = rowHeights.slice(cell.row, cell.row + cell.rowSpan).reduce((a, b) => a + b, 0) + spacing * (cell.rowSpan - 1)
            let contentX = cellX + cellBorder + pad(cell, 'left')
            let contentY = cellY + cellBorder + pad(cell, 'top')
            let valign = cell.format.verticalAlignment || 0
            let contentHeight = item.flow.y
            let roomY = cellHeight - 2 * cellBorder - pad(cell, 'top') - pad(cell, 'bottom')
            if(valign === F.ALIGN_MIDDLE) contentY += Math.max(0, (roomY - contentHeight) / 2)
            else if(valign === F.ALIGN_BOTTOM) contentY += Math.max(0, roomY - contentHeight)
            // the cell's content moved to its place
            let shift = (b) => {
                b.top += contentY
                b.bottom += contentY
                b.left += contentX
                if(b.marker){
                    b.marker.x += contentX
                    b.marker.y += contentY
                }
                for(let line of b.lines){
                    line.top += contentY
                    line.x += contentX
                    for(let it of line.items) it.x += contentX
                }
            }
            for(let b of item.flow.blocks){
                let moved = copyBlock(b, b.index, b.format)
                shift(moved)
                blocks.push(moved)
            }
            for(let t of item.flow.tables){
                t.x += contentX
                t.y += contentY
                for(let c of t.cells){
                    c.x += contentX
                    c.y += contentY
                }
                tables.push(t)
            }
            lineCount += item.flow.lineCount
            cells.push({ cell, x: cellX, y: cellY, width: item.cellWidth, height: cellHeight, background: cell.format.background || null, border: cellBorder })
        }
        return { entry, x, y, width, height, border, borderColor: format.borderBrush || '#808080', borderStyle: format.borderStyle || 'outset',
            background: format.background || null, cells, blocks, tables, lineCount }
    }

    // a block from y at x0 in the width of its frame (QTextDocumentLayoutPrivate::layoutBlock)
    layoutBlock(index, x0, y, textWidth, alignWidth){
        let doc = this.doc
        let o = this.o
        let format = doc.blockFormat(index)
        let start = doc.blockStart(index)
        let list = format.list || null
        let indent = ((format.indent || 0) + (list ? (list.format.indent || 0) : 0)) * INDENT_WIDTH
        let left = x0 + (format.leftMargin || 0) + indent
        let right = format.rightMargin || 0
        let available = textWidth === Infinity ? Infinity : Math.max(0, textWidth - (format.leftMargin || 0) - indent - right)
        let wrap = format.nonBreakableLines ? NO_WRAP : o.wrapMode
        let alignment = 'alignment' in format ? format.alignment : o.alignment
        let tokens = this.tokens(index)
        let rawLines = this.breakLines(tokens, wrap === NO_WRAP ? Infinity : available, format.textIndent || 0, wrap)
        let blockFont = this.font(doc.blockCharFormat(index))
        let blockMetrics = this.metrics(blockFont)
        let lines = []
        let top = y
        let width = 0
        for(let li = 0; li < rawLines.length; li++){
            let raw = rawLines[li]
            let ascent = 0, descent = 0
            let any = false
            for(let item of raw.items){
                if(item.kind === 'object'){
                    ascent = Math.max(ascent, item.height)
                    any = true
                    continue
                }
                let m = this.metrics(item.font)
                let shift = item.font.valign === F.ALIGN_SUPERSCRIPT ? -m.ascent * 0.5 : item.font.valign === F.ALIGN_SUBSCRIPT ? m.descent * 0.6 : 0
                ascent = Math.max(ascent, m.ascent - shift)
                descent = Math.max(descent, m.descent + shift)
                any = true
            }
            if(!any){
                ascent = blockMetrics.ascent
                descent = blockMetrics.descent
            }
            let height = ascent + descent
            let rawHeight = Math.ceil(height)
            let lineHeight = rawHeight, lineBottom = height, adjustment = 0
            let type = format.lineHeightType || 0, value = format.lineHeight || 0
            if(type === 1){ lineHeight = rawHeight * value / 100; lineBottom = height * value / 100 }
            else if(type === 2){ lineHeight = value; lineBottom = value; adjustment = ascent - lineHeight * 4 / 5 }
            else if(type === 3){ lineHeight = Math.max(rawHeight, value); lineBottom = Math.max(height, value); adjustment = height - lineHeight }
            else if(type === 4){ lineHeight = rawHeight + value; lineBottom = height + value }
            // the natural width: without the spaces at the end
            let natural = raw.width
            for(let k = raw.items.length - 1; k >= 0 && (raw.items[k].kind === 'space' || raw.items[k].kind === 'break'); k--) natural -= raw.items[k].width
            let indentFirst = li === 0 ? (format.textIndent || 0) : 0
            let room = available === Infinity ? (alignWidth > 0 ? Math.max(0, alignWidth - (format.leftMargin || 0) - indent - right) : natural + indentFirst) : available
            let free = Math.max(0, room - indentFirst - natural)
            let x = left + indentFirst
            let justify = false
            if(alignment & F.ALIGN_RIGHT) x += free
            else if(alignment & F.ALIGN_HCENTER) x += free / 2
            else if((alignment & F.ALIGN_JUSTIFY) && !raw.forced && li < rawLines.length - 1) justify = true
            let spaces = 0
            if(justify) for(let k = 0; k < raw.items.length; k++) if(raw.items[k].kind === 'space' && raw.items.slice(k + 1).some(t => t.kind !== 'space')) spaces += raw.items[k].text.length
            let extra = justify && spaces ? free / spaces : 0
            let pen = x
            let items = []
            for(let k = 0; k < raw.items.length; k++){
                let item = raw.items[k]
                let w = item.width
                if(extra && item.kind === 'space' && raw.items.slice(k + 1).some(t => t.kind !== 'space')) w += extra * item.text.length
                items.push({ kind: item.kind, text: item.text, start: item.start, end: item.end, format: item.format, font: item.font, x: pen, width: w, height: item.height })
                pen += w
            }
            let lineStart = items.length ? items[0].start : (lines.length ? lines[lines.length - 1].end : start)
            let lineEnd = items.length ? items[items.length - 1].end : lineStart
            lines.push({ top: y - adjustment, height, lineHeight, ascent, descent, x, naturalWidth: natural, start: lineStart, end: lineEnd, items, forced: raw.forced })
            width = Math.max(width, x - x0 + natural + right)
            y += lineHeight
            if(li === rawLines.length - 1) y = Math.max(y, y - lineHeight + lineBottom)
        }
        let block = { index, start, length: doc.blocks[index].size, top, bottom: y, left, right, lines, format, marker: null, hr: 'hrWidth' in format, width }
        if(list && lines.length){
            let number = this.itemNumber(list, index) + 1
            let style = 'listStyle' in format ? format.listStyle : list.format.style
            let charFont = blockFont
            let m = this.metrics(charFont)
            let spaceWidth = this.width(' ', charFont) || charFont.pixelSize / 4
            let first = lines[0]
            let markerTop = first.top + Math.round(first.ascent) - Math.round(m.ascent)
            let fontHeight = m.ascent + m.descent
            if(style === F.LIST_DISC || style === F.LIST_CIRCLE || style === F.LIST_SQUARE){
                let size = Math.floor(fontHeight / 3)
                block.marker = { shape: style === F.LIST_DISC ? 'disc' : style === F.LIST_CIRCLE ? 'circle' : 'square',
                    x: first.x - spaceWidth - size, y: markerTop + fontHeight / 2 - size / 2, width: size, height: size, font: charFont, format: doc.blockCharFormat(index) }
            } else {
                let text = itemText(Object.assign({}, list, { format: Object.assign({}, list.format, { style }) }), number)
                if(text){
                    let w = this.width(text, charFont)
                    block.marker = { text, x: first.x - spaceWidth - w, y: markerTop, width: w, height: fontHeight, font: charFont, format: doc.blockCharFormat(index) }
                }
            }
            // a task (QTextBlockFormat::marker): a box around the marker's place moved left by a sixth of the line
            // spacing, a cross in it when checked; no bullet
            if(block.marker && (format.marker === 1 || format.marker === 2)){
                let adj = Math.floor(fontHeight / 6)
                block.marker.x -= adj
                block.marker.check = { checked: format.marker === 2, adj }
            }
        }
        return block
    }
}

// a block's layout moved by dx, dy and its positions by dpos
function relocate(block, dx, dy, dpos){
    block.top += dy
    block.bottom += dy
    block.left += dx
    block.start += dpos
    if(block.marker){
        block.marker.x += dx
        block.marker.y += dy
    }
    for(let line of block.lines){
        line.top += dy
        line.x += dx
        line.start += dpos
        line.end += dpos
        for(let item of line.items){
            item.x += dx
            item.start += dpos
            item.end += dpos
        }
    }
}

// a copy of a kept block's layout to move (the kept one stays as it is)
function copyBlock(block, index, format){
    return Object.assign({}, block, {
        index, format,
        marker: block.marker ? Object.assign({}, block.marker) : null,
        lines: block.lines.map(line => Object.assign({}, line, { items: line.items.map(item => Object.assign({}, item)) })),
    })
}

function layoutDocument(doc, options){
    return new Layouter(doc, options).layout()
}

// the line that has the position: the first line whose end is after it, else the last line of its block
function lineOf(layout, pos){
    for(let block of layout.blocks){
        if(pos < block.start || pos > block.start + block.length - 1) continue
        let lines = block.lines
        for(let i = 0; i < lines.length; i++){
            let line = lines[i]
            let last = i === lines.length - 1
            if(pos < line.end || last) return { block, line, index: i }
        }
    }
    let block = layout.blocks[layout.blocks.length - 1]
    return { block, line: block.lines[block.lines.length - 1], index: block.lines.length - 1 }
}

// the x of the position in its line (QTextLine::cursorToX)
function cursorToX(layouter, line, pos){
    for(let item of line.items){
        if(pos <= item.start) return item.x
        if(pos < item.end){
            if(item.kind === 'word' || item.kind === 'space'){
                let w = layouter.width(item.text.slice(0, pos - item.start), item.font)
                if(item.kind === 'space' && item.text.length) w = item.width * (pos - item.start) / item.text.length
                return item.x + w
            }
            return item.x
        }
    }
    let last = line.items[line.items.length - 1]
    if(!last) return line.x
    if(last.kind === 'break') return last.x
    return last.x + last.width
}

// the position nearest to x in the line (QTextLine::xToCursor, CursorBetweenCharacters)
function xToCursor(layouter, line, x){
    if(!line.items.length) return line.start
    if(x <= line.items[0].x) return line.items[0].start
    for(let item of line.items){
        if(item.kind === 'break') return item.start
        if(x >= item.x + item.width) continue
        if(item.kind === 'word' || item.kind === 'space'){
            let previous = 0
            for(let n = 1; n <= item.text.length; n++){
                let w = item.kind === 'space' ? item.width * n / item.text.length : layouter.width(item.text.slice(0, n), item.font)
                if(x < item.x + w) return item.start + (x - item.x - previous < item.x + w - x ? n - 1 : n)
                previous = w
            }
            return item.end
        }
        return x < item.x + item.width / 2 ? item.start : item.end
    }
    let last = line.items[line.items.length - 1]
    return last.kind === 'break' ? last.start : last.end
}

module.exports = { Layouter, layoutDocument, lineOf, cursorToX, xToCursor, resolveFont, displayText, itemText, INDENT_WIDTH, NO_WRAP, WORD_WRAP, WRAP_ANYWHERE, WRAP }
