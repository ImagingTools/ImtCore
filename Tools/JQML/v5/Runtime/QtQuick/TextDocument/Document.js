// QTextDocument as its piece table keeps it (qtextdocument_p.cpp of Qt 6.12): the characters are fragments of an append
// only buffer, each with its char format; the blocks cover the fragments, each ends with its separator (U+2029, or
// U+FDD0 / U+FDD1 at the start / end of a frame). The separator before a block carries the block's char format, the one
// of the first block is initialBlockCharFormat. A position is a character of the document: the last block's separator is
// the end, so length is characterCount - 1. Lists are objects a block format refers to (QTextFormat::ObjectIndex).
const F = require('./Formats')

const PARAGRAPH_SEPARATOR = '\u2029', LINE_SEPARATOR = '\u2028', BEGINNING_OF_FRAME = '\ufdd0', END_OF_FRAME = '\ufdd1'
const OBJECT_REPLACEMENT = '\ufffc', NBSP = '\u00a0'

const isBlockSeparator = (c) => c === PARAGRAPH_SEPARATOR || c === BEGINNING_OF_FRAME || c === END_OF_FRAME

let nextObjectId = 1

// QTextUndoCommand::Command
const INSERTED = 0, REMOVED = 1, CHAR_FORMAT_CHANGED = 2, BLOCK_FORMAT_CHANGED = 3, BLOCK_INSERTED = 4, BLOCK_REMOVED = 5, BLOCK_ADDED = 6,
    BLOCK_DELETED = 7, CURSOR_MOVED = 8

// a block's version: a new one each time its characters or their formats change (the layout of a block is kept by it)
let nextSignature = 1

// QTextList: a format of its own, its items are the blocks whose format refers to it
function makeList(format){
    return { $identity: true, id: nextObjectId++, format: Object.freeze(Object.assign({}, format)) }
}

class TextDocument {
    constructor(){
        this.clear()
    }

    // QTextDocumentPrivate::clear / init: one empty block
    clear(){
        // the undo history goes with the text (QTextDocumentPrivate::clear: clearUndoRedoStacks)
        let undoEnabled = this.undoEnabled === undefined ? true : this.undoEnabled
        this.undoEnabled = false
        this.editBlock = 0
        this.editBlockCursorPosition = -1
        this.changeFrom = -1
        this.changeLength = 0
        this.clearUndo()
        this.text = ''
        this.fragments = []
        this.blocks = []
        this.initialBlockCharFormat = F.EMPTY
        this.rootFrameFormat = F.EMPTY
        this.title = ''
        // the text direction of the default text option (QTextOption::textDirection): null is automatic
        if(this.defaultTextDirection === undefined) this.defaultTextDirection = null
        this.insertBlock(0, F.EMPTY, F.EMPTY)
        this.undoEnabled = undoEnabled
    }

    // ---- sizes and positions ----

    // the first positions of the blocks and of the fragments, made again after a change of the piece table
    index(){
        if(this.$idx) return this.$idx
        let blockStarts = new Array(this.blocks.length)
        let p = 0
        for(let i = 0; i < this.blocks.length; i++){
            blockStarts[i] = p
            p += this.blocks[i].size
        }
        let total = p
        let fragStarts = new Array(this.fragments.length)
        p = 0
        for(let i = 0; i < this.fragments.length; i++){
            fragStarts[i] = p
            p += this.fragments[i].size
        }
        this.$idx = { blockStarts, fragStarts, total }
        return this.$idx
    }

    // the last index whose start is not after pos
    static search(starts, pos){
        let lo = 0, hi = starts.length - 1
        while(lo < hi){
            let mid = (lo + hi + 1) >> 1
            if(starts[mid] <= pos) lo = mid
            else hi = mid - 1
        }
        return lo
    }

    characterCount(){
        return this.index().total
    }

    length(){
        return this.characterCount() - 1
    }

    isEmpty(){
        return this.blocks.length === 1 && this.blocks[0].size === 1
    }

    blockStart(index){
        let starts = this.index().blockStarts
        return index < starts.length ? starts[index] : this.index().total
    }

    // the block that has the character at pos (the end: the last block)
    blockIndexAt(pos){
        let idx = this.index()
        if(pos >= idx.total) return this.blocks.length - 1
        return TextDocument.search(idx.blockStarts, Math.max(0, pos))
    }

    // the fragment that has the character at pos: its index and its first position
    fragmentAt(pos){
        let idx = this.index()
        if(pos >= idx.total || !this.fragments.length) return { index: this.fragments.length, start: idx.total }
        let i = TextDocument.search(idx.fragStarts, Math.max(0, pos))
        return { index: i, start: idx.fragStarts[i] }
    }

    fragmentStart(index){
        let starts = this.index().fragStarts
        return index < starts.length ? starts[index] : this.index().total
    }

    characterAt(pos){
        let { index, start } = this.fragmentAt(pos)
        let frag = this.fragments[index]
        return frag ? this.text[frag.stringPosition + pos - start] : ''
    }

    // the characters of [start, end) as the buffer has them (QTextCursor::selectedText)
    textRange(start, end){
        let out = ''
        if(end <= start || !this.fragments.length) return out
        let { index: first, start: firstStart } = this.fragmentAt(start)
        let pos = firstStart
        for(let i = first; i < this.fragments.length; i++){
            let frag = this.fragments[i]
            let fragEnd = pos + frag.size
            if(fragEnd > start && pos < end){
                let from = Math.max(start, pos), to = Math.min(end, fragEnd)
                out += this.text.substr(frag.stringPosition + from - pos, to - from)
            }
            if(fragEnd >= end) break
            pos = fragEnd
        }
        return out
    }

    // QTextDocument::toRawText: the text without the last separator
    rawText(){
        return this.textRange(0, this.length())
    }

    // QTextDocument::toPlainText
    plainText(){
        return this.rawText().replace(/[\ufdd0\ufdd1\u2029\u2028]/g, '\n').replace(/\u00a0/g, ' ')
    }

    blockText(index){
        let start = this.blockStart(index)
        return this.textRange(start, start + this.blocks[index].size - 1)
    }

    blockLength(index){
        return this.blocks[index].size
    }

    // the separator that ends the block (QTextBlock::next starts after it)
    blockSeparator(index){
        let start = this.blockStart(index)
        return this.characterAt(start + this.blocks[index].size - 1)
    }

    // the separator before the block ('' for the first)
    separatorBefore(index){
        return index === 0 ? '' : this.characterAt(this.blockStart(index) - 1)
    }

    blockFormat(index){
        return this.blocks[index].format
    }

    // QTextDocumentPrivate::blockCharFormatIndex: the format of the separator before the block
    blockCharFormat(index){
        let start = this.blockStart(index)
        if(start === 0) return this.initialBlockCharFormat
        let { index: f } = this.fragmentAt(start - 1)
        return this.fragments[f].format
    }

    // the fragments of a block without its separator (QTextBlock::iterator: the pieces of one format next to each other
    // are one fragment): { text, format, start }
    blockFragments(index){
        let start = this.blockStart(index)
        let end = start + this.blocks[index].size - 1
        let out = []
        if(end <= start) return out
        let { index: first, start: firstStart } = this.fragmentAt(start)
        let pos = firstStart
        for(let i = first; i < this.fragments.length; i++){
            let frag = this.fragments[i]
            let fragEnd = pos + frag.size
            if(pos >= end) break
            if(fragEnd > start){
                let from = Math.max(start, pos), to = Math.min(end, fragEnd)
                if(to > from){
                    let text = this.text.substr(frag.stringPosition + from - pos, to - from)
                    let last = out[out.length - 1]
                    if(last && (last.format === frag.format || F.equal(last.format, frag.format))) last.text += text
                    else out.push({ text, format: frag.format, start: from })
                }
            }
            pos = fragEnd
        }
        return out
    }

    // the format of the character at pos
    charFormatAt(pos){
        let { index } = this.fragmentAt(pos)
        let frag = this.fragments[index]
        return frag ? frag.format : F.EMPTY
    }

    // ---- lists ----

    listOf(index){
        return this.blocks[index].format.list || null
    }

    listItems(list){
        let items = []
        for(let i = 0; i < this.blocks.length; i++) if(this.blocks[i].format.list === list) items.push(i)
        return items
    }

    // QTextList::itemNumber: the index of the block among the items, -1 when it is none
    itemNumber(list, index){
        return this.listItems(list).indexOf(index)
    }

    createListObject(format){
        return makeList(format)
    }

    // ---- tables (QTextTable): a cell starts after a U+FDD0 of the table, the table ends at its U+FDD1; the separators
    // have the char format of the cell (objectType 'tableCell', table, rowSpan, colSpan, the cell's format) ----

    // the tables of the document in the order they start, each { table, start (position of its first U+FDD0), end
    // (position of its U+FDD1), markers (positions of the U+FDD0 of its cells) }
    tables(){
        let found = new Map()
        let pos = 0
        for(let frag of this.fragments){
            if(frag.size === 1 && frag.format.table){
                let c = this.text[frag.stringPosition]
                let t = frag.format.table
                let entry = found.get(t)
                if(!entry){
                    entry = { table: t, start: pos, end: -1, markers: [] }
                    found.set(t, entry)
                }
                if(c === BEGINNING_OF_FRAME) entry.markers.push(pos)
                else if(c === END_OF_FRAME) entry.end = pos
            }
            pos += frag.size
        }
        return Array.from(found.values()).filter(e => e.end >= 0)
    }

    // the grid of a table (QTextTablePrivate::update): its cells at their row and column, each { marker, row, column,
    // rowSpan, columnSpan, format, first (the first position of its text), last (the position after it) }
    tableGrid(entry){
        let columns = entry.table.format.columns || 1
        let cells = entry.markers.map((marker, i) => {
            let format = this.charFormatAt(marker)
            let next = i + 1 < entry.markers.length ? entry.markers[i + 1] : entry.end
            return { marker, row: 0, column: 0, rowSpan: format.rowSpan || 1, columnSpan: format.columnSpan || 1, format, first: marker + 1, last: next }
        })
        let rows = Math.ceil(cells.length / columns)
        let grid = new Array(rows * columns).fill(null)
        let at = 0
        for(let cell of cells){
            while(at < grid.length && grid[at]) at++
            let r = Math.floor(at / columns), c = at % columns
            cell.row = r
            cell.column = c
            if(r + cell.rowSpan > rows){
                grid.length = (r + cell.rowSpan) * columns
                for(let k = rows * columns; k < grid.length; k++) grid[k] = null
                rows = r + cell.rowSpan
            }
            for(let i = 0; i < cell.rowSpan; i++) for(let j = 0; j < cell.columnSpan && c + j < columns; j++) grid[(r + i) * columns + c + j] = cell
        }
        return { rows, columns, cells, grid, cellAt: (r, c) => r >= 0 && c >= 0 && r < rows && c < columns ? grid[r * columns + c] : null }
    }

    // the table entry and cell of the innermost table cell that has the position, or null
    cellAt(pos){
        let best = null
        for(let entry of this.tables()){
            if(pos <= entry.start || pos > entry.end) continue
            if(best && entry.start < best.entry.start) continue
            let grid = this.tableGrid(entry)
            for(let cell of grid.cells){
                if(pos >= cell.first && pos <= cell.last){
                    best = { entry, grid, cell }
                    break
                }
            }
        }
        return best
    }

    // QTextTablePrivate::createTable: the cells' U+FDD0 and the table's U+FDD1 at pos
    insertTable(pos, rows, columns, format){
        let table = { $identity: true, id: nextObjectId++, format: Object.freeze(Object.assign({}, format, { columns })) }
        let cellFormat = Object.freeze({ objectType: 'tableCell', table })
        this.beginEditBlock()
        for(let i = 0; i < rows * columns; i++) this.insertBlock(pos++, F.EMPTY, cellFormat, BEGINNING_OF_FRAME)
        this.insertBlock(pos, F.EMPTY, cellFormat, END_OF_FRAME)
        this.endEditBlock()
        return table
    }

    // QTextTable::mergeCells of the empty cells of an imported table: the cells under the merged one go, the merged
    // one spans them
    mergeCells(table, row, column, numRows, numColumns){
        let entry = this.tables().find(e => e.table === table)
        if(!entry) return
        let grid = this.tableGrid(entry)
        let cell = grid.cellAt(row, column)
        if(!cell || cell.row !== row || cell.column !== column) return
        numRows = Math.min(numRows, grid.rows - row)
        numColumns = Math.min(numColumns, grid.columns - column)
        if(numRows < cell.rowSpan || numColumns < cell.columnSpan) return
        this.beginEditBlock()
        try { this.mergeCellsOf(table, cell, row, column, numRows, numColumns, grid) } finally { this.endEditBlock() }
    }

    mergeCellsOf(table, cell, row, column, numRows, numColumns, grid){
        let doomed = new Set()
        for(let r = row; r < row + numRows; r++){
            for(let c = column; c < column + numColumns; c++){
                let other = grid.cellAt(r, c)
                if(other && other !== cell) doomed.add(other)
            }
        }
        let markers = Array.from(doomed).map(c => c.marker).sort((a, b) => b - a)
        // the content of a merged cell goes into the cell that takes it over (empty when importing)
        for(let marker of markers){
            let other = Array.from(doomed).find(c => c.marker === marker)
            let content = other.last - other.first
            if(content > 0){
                let rawText = this.textRange(other.first, other.last)
                this.remove(other.marker, content + 1)
                let target = this.tableGrid(this.tables().find(e => e.table === table)).cells.find(c => c.marker === cell.marker)
                if(rawText) this.insertString(target.last, ' ' + rawText.replace(/[\u2029\ufdd0\ufdd1]/g, ' '), this.charFormatAt(Math.max(target.first, target.last - 1)))
            } else this.remove(marker, 1)
        }
        let at = this.tables().find(e => e.table === table)
        let origin = this.tableGrid(at).cells.find(c => c.row === row && c.column === column)
        if(origin) this.setCharFormat(origin.marker, 1, F.with_(origin.format, { rowSpan: numRows, columnSpan: numColumns }), false)
    }

    // the cell format of a cell (QTextTableCell::setFormat): the format of its U+FDD0, the table kept
    setCellFormat(marker, format){
        let old = this.charFormatAt(marker)
        this.setCharFormat(marker, 1, F.with_(format, { objectType: 'tableCell', table: old.table, rowSpan: old.rowSpan, columnSpan: old.columnSpan }), false)
    }

    // ---- the piece table ----

    split(pos){
        let { index, start } = this.fragmentAt(pos)
        if(index >= this.fragments.length || start === pos) return
        this.$idx = null
        let frag = this.fragments[index]
        let head = pos - start
        this.fragments.splice(index, 1,
            { size: head, stringPosition: frag.stringPosition, format: frag.format },
            { size: frag.size - head, stringPosition: frag.stringPosition + head, format: frag.format })
    }

    // QTextDocumentPrivate::unite: the fragment and the next one are one when the format and the buffer go on
    unite(index){
        let f = this.fragments[index], n = this.fragments[index + 1]
        if(!f || !n) return false
        if(F.equal(f.format, n.format) && f.stringPosition + f.size === n.stringPosition){
            if(isBlockSeparator(this.text[f.stringPosition]) || isBlockSeparator(this.text[n.stringPosition])) return false
            this.$idx = null
            this.fragments.splice(index, 2, { size: f.size + n.size, stringPosition: f.stringPosition, format: f.format })
            return true
        }
        return false
    }

    // QTextDocumentPrivate::insert_string: the characters of the buffer at strPos at pos (no separators)
    insertStringAt(pos, strPos, length, format){
        this.split(pos)
        let { index } = this.fragmentAt(pos)
        let b = this.blockIndexAt(pos)
        this.$idx = null
        this.fragments.splice(index, 0, { size: length, stringPosition: strPos, format })
        this.blocks[b].size += length
        this.blocks[b].sig = nextSignature++
        if(index > 0) this.unite(index - 1)
        this.changed(pos, length)
    }

    // QTextDocumentPrivate::insert: text without separators at pos in the format (an Inserted step of the undo history)
    insertString(pos, str, format){
        if(!str.length) return
        let strPos = this.text.length
        this.text += str
        this.insertStringAt(pos, strPos, str.length, format)
        this.appendUndo({ command: INSERTED, pos, strPos, length: str.length, format })
    }

    // QTextDocumentPrivate::insert_block: the separator of the buffer at strPos at pos; afterBlock: the new block is
    // the one after the separator (BlockRemoved), else the one before it (BlockDeleted: an empty block put back)
    insertBlockAt(pos, strPos, charFormat, blockFormat, afterBlock = true){
        this.split(pos)
        let { index } = this.fragmentAt(pos)
        let b0 = this.blocks.length ? this.blockIndexAt(pos) : -1
        let start0 = this.blocks.length ? this.blockStart(b0) : 0
        this.$idx = null
        this.fragments.splice(index, 0, { size: 1, stringPosition: strPos, format: charFormat })
        if(!this.blocks.length){
            this.blocks.push({ size: 1, format: blockFormat, userState: -1, sig: nextSignature++ })
            return
        }
        if(!afterBlock && pos === start0){
            // the empty block before the block at pos
            this.blocks.splice(b0, 0, { size: 1, format: blockFormat, userState: -1, sig: nextSignature++ })
            this.changed(pos, 1)
            return
        }
        let block = this.blocks[b0]
        let rest = block.size - (pos - start0)
        block.size = pos - start0 + 1
        block.sig = nextSignature++
        this.blocks.splice(b0 + 1, 0, { size: rest, format: blockFormat, userState: -1, sig: nextSignature++ })
        this.changed(pos, 1)
    }

    // QTextDocumentPrivate::insertBlock: a separator at pos with the char format; the block after it has the block format
    insertBlock(pos, blockFormat, charFormat, separator = PARAGRAPH_SEPARATOR){
        this.beginEditBlock()
        let strPos = this.text.length
        this.text += separator
        this.insertBlockAt(pos, strPos, charFormat, blockFormat, true)
        this.appendUndo({ command: BLOCK_INSERTED, pos, strPos, length: 1, format: charFormat, blockFormat })
        this.endEditBlock()
    }

    // QTextDocumentPrivate::remove (move to -1): the characters go; a removed separator joins its block with the next,
    // the block keeping its format, or drops the block when it was empty. Each fragment is a Removed (or BlockRemoved,
    // BlockDeleted) step of the undo history
    remove(pos, length){
        if(length <= 0) return
        this.split(pos)
        this.split(pos + length)
        let left = length
        let w = -1
        while(left > 0){
            let { index } = this.fragmentAt(pos)
            let frag = this.fragments[index]
            let b = this.blockIndexAt(pos)
            let blockEnd = this.blockStart(b) + this.blocks[b].size - 1
            this.$idx = null
            let command
            if(pos !== blockEnd){
                this.blocks[b].size -= frag.size
                this.blocks[b].sig = nextSignature++
                command = { command: REMOVED, pos, strPos: frag.stringPosition, length: frag.size, format: frag.format }
            } else {
                // the separator of block b
                let removed = this.removeBlockOf(b, true)
                command = { command: removed.deleted ? BLOCK_DELETED : BLOCK_REMOVED, pos, strPos: frag.stringPosition, length: 1, format: frag.format, blockFormat: removed.format }
            }
            this.fragments.splice(index, 1)
            this.changed(pos, 0)
            this.appendUndo(command)
            w = index - 1
            left -= frag.size
        }
        if(w >= 0) this.unite(w)
    }

    // QTextDocumentPrivate::remove_block: the separator of block b goes; an empty block goes itself when deleteEmpty
    // (BlockAdded), else the next block joins it. The format of the block that went
    removeBlockOf(b, deleteEmpty){
        this.$idx = null
        if(this.blocks[b].size === 1 && deleteEmpty){
            let format = this.blocks[b].format
            this.blocks.splice(b, 1)
            if(this.blocks[b]) this.blocks[b].sig = nextSignature++
            return { format, deleted: true }
        }
        let next = this.blocks[b + 1]
        this.blocks[b].size += next.size - 1
        this.blocks[b].userState = next.userState
        this.blocks[b].sig = nextSignature++
        this.blocks.splice(b + 1, 1)
        return { format: next.format, deleted: false }
    }

    // the separator at pos removed as an undo step does (no step of its own)
    removeSeparator(pos, deleteEmpty){
        this.split(pos)
        this.split(pos + 1)
        let { index } = this.fragmentAt(pos)
        let b = this.blockIndexAt(pos)
        let removed = this.removeBlockOf(b, deleteEmpty)
        this.fragments.splice(index, 1)
        this.$idx = null
        if(index > 0) this.unite(index - 1)
        this.changed(pos, 0)
        return removed
    }

    // QTextDocumentPrivate::setCharFormat: the format of [pos, pos + length) set or merged; pos -1 is the char format of
    // the first block. A CharFormatChanged step for each fragment
    setCharFormat(pos, length, format, merge){
        this.beginEditBlock()
        let apply = (old) => merge ? F.merge(old, format) : format
        // the blocks whose characters or char format change
        let firstBlock = this.blockIndexAt(Math.max(0, pos)), lastBlock = this.blockIndexAt(pos + length)
        for(let b = firstBlock; b <= lastBlock && b < this.blocks.length; b++) this.blocks[b].sig = nextSignature++
        if(pos === -1){
            this.initialBlockCharFormat = apply(this.initialBlockCharFormat)
            pos++
            length--
        }
        if(length > 0){
            let end = pos + length
            this.split(pos)
            this.split(end)
            let p = 0
            for(let i = 0; i < this.fragments.length; i++){
                let frag = this.fragments[i]
                if(p >= pos && p < end){
                    this.fragments[i] = Object.assign({}, frag, { format: apply(frag.format) })
                    this.appendUndo({ command: CHAR_FORMAT_CHANGED, pos: p, length: frag.size, format: frag.format, partOfBlock: true })
                }
                p += frag.size
            }
            // the fragment before the range with the first one, the one at the end with the next
            if(pos > 0) this.unite(this.fragmentAt(pos - 1).index)
            let at = this.fragmentAt(end).index
            if(at < this.fragments.length) this.unite(at)
            this.changed(pos, length)
        }
        this.endEditBlock()
    }

    // QTextDocumentPrivate::setBlockFormat: a BlockFormatChanged step
    setBlockFormat(index, format){
        this.beginEditBlock()
        let old = this.blocks[index].format
        this.blocks[index].format = format
        this.blocks[index].sig = nextSignature++
        this.appendUndo({ command: BLOCK_FORMAT_CHANGED, pos: this.blockStart(index), length: 1, format: old, partOfBlock: true })
        this.changed(this.blockStart(index), this.blocks[index].size)
        this.endEditBlock()
    }

    // the block char format of a block: the format of its separator before
    setBlockCharFormat(index, format, merge){
        let start = this.blockStart(index)
        this.setCharFormat(start - 1, 1, format, merge)
    }

    // ---- the undo history (QTextUndoCommand, QTextDocumentPrivate::appendUndoItem, undoRedo) ----

    // the range of the document an edit changed (docChangeFrom, docChangeLength)
    changed(pos, length){
        if(this.changeFrom < 0){
            this.changeFrom = pos
            this.changeLength = length
            return
        }
        let start = Math.min(this.changeFrom, pos)
        let end = Math.max(this.changeFrom + this.changeLength, pos + length)
        this.changeFrom = start
        this.changeLength = end - start
    }

    // QTextDocumentPrivate::beginEditBlock; QTextCursor::beginEditBlock gives the cursor's position, a step that changes
    // the document elsewhere then puts a CursorMoved step before it
    beginEditBlock(cursorPosition = -1){
        if(this.editBlock === 0 && cursorPosition >= 0) this.editBlockCursorPosition = cursorPosition
        this.editBlock++
    }

    endEditBlock(){
        if(--this.editBlock) return
        if(this.undoEnabled && this.undoState > 0){
            let last = this.undoStack[this.undoState - 1]
            if(last.blockPart) last.blockEnd = true
        }
        this.editBlockCursorPosition = -1
        if(this.undoEnabled) this.modified = this.undoState !== this.modifiedState
    }

    // QTextUndoCommand::tryMerge: inserts that go on, Delete and Backspace removals that go on
    static tryMerge(last, c){
        if(last.command !== c.command) return false
        let sameFormat = last.format === c.format || F.equal(last.format, c.format)
        if(c.command === INSERTED && last.pos + last.length === c.pos && last.strPos + last.length === c.strPos && sameFormat){
            last.length += c.length
            return true
        }
        if(c.command === REMOVED && last.pos === c.pos && last.strPos + last.length === c.strPos && sameFormat){
            last.length += c.length
            return true
        }
        if(c.command === REMOVED && c.pos + c.length === last.pos && c.strPos + c.length === last.strPos && sameFormat){
            let length = last.length
            Object.assign(last, c)
            last.length += length
            return true
        }
        return false
    }

    // QTextDocumentPrivate::appendUndoItem
    appendUndo(c){
        if(!this.undoEnabled) return
        c.blockPart = this.editBlock !== 0 || !!c.partOfBlock
        c.blockEnd = false
        delete c.partOfBlock
        if(this.undoState < this.undoStack.length) this.undoStack.length = this.undoState
        if(this.editBlock !== 0 && this.editBlockCursorPosition >= 0){
            if(c.pos !== this.editBlockCursorPosition){
                this.undoStack.push({ command: CURSOR_MOVED, pos: this.editBlockCursorPosition, length: 0, blockPart: true, blockEnd: false })
                this.undoState++
            }
            this.editBlockCursorPosition = -1
        }
        if(this.undoStack.length && this.modified){
            let last = this.undoStack[this.undoState - 1]
            if((last.blockPart && c.blockPart && !last.blockEnd) || (!c.blockPart && !last.blockPart)
                || (c.command === INSERTED && last.command === c.command && last.blockPart && !c.blockPart)){
                if(TextDocument.tryMerge(last, c)) return
            }
        }
        if(this.modifiedState > this.undoState) this.modifiedState = -1
        this.undoStack.push(c)
        this.undoState++
        if(this.editBlock === 0) this.modified = this.undoState !== this.modifiedState
    }

    isUndoAvailable(){ return this.undoEnabled && this.undoState > 0 }
    isRedoAvailable(){ return this.undoEnabled && this.undoState < this.undoStack.length }

    // QTextDocumentPrivate::clearUndoRedoStacks
    clearUndo(){
        this.undoStack = []
        this.undoState = 0
        this.modifiedState = 0
        this.modified = false
    }

    // QTextDocumentPrivate::undoRedo: the steps of the last edit block undone (or redone); the position the cursor goes
    // to (after the text a step put back, at the text it took away), -1 for none
    undoRedo(undo){
        if(!this.undoEnabled || (undo && this.undoState === 0) || (!undo && this.undoState === this.undoStack.length)) return -1
        this.undoEnabled = false
        this.changeFrom = -1
        this.changeLength = 0
        let editPos = -1, editLength = -1
        for(;;){
            if(undo) this.undoState--
            let c = this.undoStack[this.undoState]
            switch(c.command){
                case INSERTED:
                    this.remove(c.pos, c.length)
                    c.command = REMOVED
                    editPos = c.pos
                    editLength = 0
                    break
                case REMOVED:
                    this.insertStringAt(c.pos, c.strPos, c.length, c.format)
                    c.command = INSERTED
                    if(editPos !== c.pos) editLength = 0
                    editPos = c.pos
                    editLength += c.length
                    break
                case BLOCK_INSERTED: case BLOCK_ADDED: {
                    // the separator removed again; the format of the block that goes is the one to put back
                    let removed = this.removeSeparator(c.pos, c.command === BLOCK_ADDED)
                    c.blockFormat = removed.format
                    c.command = c.command === BLOCK_INSERTED ? BLOCK_REMOVED : BLOCK_DELETED
                    editPos = c.pos
                    editLength = 0
                    break
                }
                case BLOCK_REMOVED: case BLOCK_DELETED:
                    this.insertBlockAt(c.pos, c.strPos, c.format, c.blockFormat, c.command === BLOCK_REMOVED)
                    c.command = c.command === BLOCK_REMOVED ? BLOCK_INSERTED : BLOCK_ADDED
                    if(editPos !== c.pos) editLength = 0
                    editPos = c.pos
                    editLength += 1
                    break
                case CHAR_FORMAT_CHANGED: {
                    let old = this.charFormatAt(c.pos)
                    this.setCharFormat(c.pos, c.length, c.format, false)
                    c.format = old
                    if(editPos !== c.pos) editLength = 0
                    editPos = c.pos
                    editLength += c.length
                    break
                }
                case BLOCK_FORMAT_CHANGED: {
                    let b = this.blockIndexAt(c.pos)
                    let old = this.blocks[b].format
                    this.blocks[b].format = c.format
                    this.blocks[b].sig = nextSignature++
                    c.format = old
                    this.changed(this.blockStart(b), this.blocks[b].size)
                    editPos = -1
                    break
                }
                case CURSOR_MOVED:
                    editPos = c.pos
                    editLength = 0
                    break
            }
            if(!undo) this.undoState++
            let inBlock = this.undoState > 0 && this.undoState < this.undoStack.length
                && this.undoStack[this.undoState].blockPart && this.undoStack[this.undoState - 1].blockPart
                && !this.undoStack[this.undoState - 1].blockEnd
            if(!inBlock) break
        }
        this.undoEnabled = true
        let position = -1
        if(editPos >= 0) position = editPos + editLength
        else if(this.changeFrom >= 0) position = Math.min(this.changeFrom + this.changeLength, this.length() - 1)
        this.modified = this.undoState !== this.modifiedState
        return position
    }

    undo(){ return this.undoRedo(true) }
    redo(){ return this.undoRedo(false) }

    // ---- copies ----

    clone(){
        let doc = Object.create(TextDocument.prototype)
        doc.text = this.text
        doc.fragments = this.fragments.map(f => Object.assign({}, f))
        doc.blocks = this.blocks.map(b => Object.assign({}, b))
        doc.initialBlockCharFormat = this.initialBlockCharFormat
        doc.rootFrameFormat = this.rootFrameFormat
        doc.title = this.title
        doc.defaultTextDirection = this.defaultTextDirection
        doc.undoEnabled = true
        doc.editBlock = 0
        doc.editBlockCursorPosition = -1
        doc.changeFrom = -1
        doc.changeLength = 0
        doc.clearUndo()
        return doc
    }
}

module.exports = {
    TextDocument, makeList, isBlockSeparator,
    PARAGRAPH_SEPARATOR, LINE_SEPARATOR, BEGINNING_OF_FRAME, END_OF_FRAME, OBJECT_REPLACEMENT, NBSP,
}
