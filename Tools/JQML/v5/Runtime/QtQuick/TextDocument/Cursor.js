// QTextCursor (qtextcursor.cpp of Qt 6.12): a position and an anchor in a document, the editing operations at them
const F = require('./Formats')
const { PARAGRAPH_SEPARATOR, BEGINNING_OF_FRAME, END_OF_FRAME, OBJECT_REPLACEMENT } = require('./Document')

// QTextCursor::MoveOperation
const MOVE = { NoMove: 0, Start: 1, Up: 2, StartOfLine: 3, StartOfBlock: 4, StartOfWord: 5, PreviousBlock: 6, PreviousCharacter: 7,
    PreviousWord: 8, Left: 9, WordLeft: 10, End: 11, Down: 12, EndOfLine: 13, EndOfWord: 14, EndOfBlock: 15, NextBlock: 16,
    NextCharacter: 17, NextWord: 18, Right: 19, WordRight: 20, NextCell: 21, PreviousCell: 22, NextRow: 23, PreviousRow: 24 }

class TextCursor {
    constructor(doc, position = 0){
        this.doc = doc
        this.position = position
        this.anchor = position
        // QTextCursorPrivate::currentCharFormat: a format set on the cursor without a selection (mergeCharFormat)
        this.currentCharFormat = null
    }

    hasSelection(){ return this.position !== this.anchor }
    selectionStart(){ return Math.min(this.position, this.anchor) }
    selectionEnd(){ return Math.max(this.position, this.anchor) }

    setPosition(pos, keepAnchor = false){
        let max = this.doc.length()
        pos = Math.max(0, Math.min(pos, max))
        this.position = pos
        if(!keepAnchor) this.anchor = pos
        this.currentCharFormat = null
    }

    block(){
        return this.doc.blockIndexAt(this.position)
    }

    blockFormat(){
        return this.doc.blockFormat(this.block())
    }

    blockCharFormat(){
        return this.doc.blockCharFormat(this.block())
    }

    // QTextCursor::charFormat: the format of the character before the cursor; at the start of a block that is not empty the
    // one after it
    charFormat(){
        if(this.currentCharFormat) return this.currentCharFormat
        let doc = this.doc
        let b = this.block()
        let start = doc.blockStart(b)
        let pos = this.position === start && doc.blockLength(b) > 1 ? this.position : this.position - 1
        let format = pos === -1 ? doc.initialBlockCharFormat : doc.charFormatAt(pos)
        // the object of the format (QTextFormat::ObjectIndex: the table of a cell's separator) is not the text's
        return F.without(format, 'objectIndex', 'table')
    }

    // QTextCursorPrivate::remove: a selection over the edge of a table takes the whole table (adjustCursor); one over
    // several cells of a table clears the cells of the rectangle between them (clearCells)
    removeSelection(){
        if(this.anchor === this.position) return
        let doc = this.doc
        let start = this.selectionStart(), end = this.selectionEnd()
        let extended = false
        for(let changed = true; changed;){
            changed = false
            for(let e of doc.tables()){
                // the positions in the table's cells
                let inside = (p) => p > e.start && p <= e.end
                if(inside(start) !== inside(end)){
                    let s = Math.min(start, e.start), t = Math.max(end, e.end + 1)
                    if(s !== start || t !== end){
                        start = s
                        end = t
                        changed = true
                        extended = true
                    }
                }
            }
        }
        if(!extended){
            let complex = this.complexSelection()
            if(complex){
                let cells = complex.cells.slice().sort((x, y) => y.marker - x.marker)
                for(let cell of cells) if(cell.last > cell.first) doc.remove(cell.first, cell.last - cell.first)
                this.removed = null
                let entry = doc.tables().find(e => e.table === complex.table)
                let after = doc.tableGrid(entry).cellAt(complex.positionCell.row, complex.positionCell.column)
                this.position = this.anchor = after ? after.first : start
                this.currentCharFormat = null
                return
            }
        }
        doc.remove(start, end - start)
        this.removed = { start, end }
        this.position = this.anchor = start
        this.currentCharFormat = null
    }

    // QTextCursor::hasComplexSelection / selectedTableCells: anchor and position in two cells of one table; the cells of
    // the rectangle between them, row by row, a spanned cell once
    complexSelection(){
        if(this.anchor === this.position) return null
        let doc = this.doc
        let a = doc.cellAt(this.anchor), p = doc.cellAt(this.position)
        if(!a || !p || a.entry.table !== p.entry.table || a.cell.marker === p.cell.marker) return null
        let grid = a.grid
        let rowStart = Math.min(a.cell.row, p.cell.row), colStart = Math.min(a.cell.column, p.cell.column)
        let rowEnd = Math.max(a.cell.row + a.cell.rowSpan, p.cell.row + p.cell.rowSpan)
        let colEnd = Math.max(a.cell.column + a.cell.columnSpan, p.cell.column + p.cell.columnSpan)
        let cells = []
        for(let r = rowStart; r < rowEnd; r++){
            for(let c = colStart; c < colEnd; c++){
                let cell = grid.cellAt(r, c)
                if(!cell || (cell.rowSpan !== 1 && cell.row !== r) || (cell.columnSpan !== 1 && cell.column !== c)) continue
                if(!cells.includes(cell)) cells.push(cell)
            }
        }
        return { table: a.entry.table, cells, anchorCell: a.cell, positionCell: p.cell }
    }

    // QTextCursor::selectedText: the text of the selection, of the cells of a complex one one after the other
    selectedText(){
        let complex = this.complexSelection()
        if(!complex) return this.doc.textRange(this.selectionStart(), this.selectionEnd())
        return complex.cells.map(cell => this.doc.textRange(cell.first, cell.last)).join('')
    }

    // QTextCursorPrivate::canDelete: the character is no frame's (a table's cell or end)
    canDelete(pos){
        let format = this.doc.charFormatAt(pos)
        if(!format.table) return true
        let c = this.doc.characterAt(pos)
        return c !== BEGINNING_OF_FRAME && c !== END_OF_FRAME
    }

    // QTextCursor::removeSelectedText: one edit block
    removeSelectedText(){
        if(this.anchor === this.position) return
        this.doc.beginEditBlock()
        this.removeSelection()
        this.doc.endEditBlock()
    }

    // QTextCursor::deleteChar / deletePreviousChar: the character after (before) the cursor, a step of its own that the
    // next one may join (no edit block)
    deleteChar(previous){
        if(this.anchor !== this.position) return this.removeSelectedText()
        let pos = previous ? this.position - 1 : this.position
        if(pos < 0 || pos >= this.doc.length() || !this.canDelete(pos)) return
        let text = this.doc.textRange(Math.max(0, pos - 1), Math.min(this.doc.length(), pos + 2))
        let from = pos, to = pos + 1
        // a surrogate pair is one character
        let at = pos - Math.max(0, pos - 1)
        let isHigh = (c) => c >= '\ud800' && c <= '\udbff', isLow = (c) => c >= '\udc00' && c <= '\udfff'
        if(previous && isLow(text[at]) && isHigh(text[at - 1])) from--
        else if(!previous && isHigh(text[at]) && isLow(text[at + 1])) to++
        this.doc.remove(from, to - from)
        this.position = this.anchor = from
        this.currentCharFormat = null
    }

    // QTextCursor::insertText: the separators in the text make new blocks of the current block format
    insertText(text, format){
        if(format === undefined) format = F.without(this.charFormat(), 'objectType')
        format = F.without(format, 'objectIndex')
        // an edit block when a selection goes or a separator makes a block
        let editBlock = false
        if(this.anchor !== this.position){
            editBlock = true
            this.doc.beginEditBlock()
            this.removeSelection()
        }
        if(!text.length){
            if(editBlock) this.doc.endEditBlock()
            return
        }
        let blockFormat = this.blockFormat()
        let chunk = ''
        let flush = () => {
            if(!chunk) return
            this.doc.insertString(this.position, chunk, format)
            this.position += chunk.length
            chunk = ''
        }
        for(let i = 0; i < text.length; i++){
            let ch = text[i]
            if(ch === '\r' && i + 1 < text.length && text[i + 1] === '\n'){
                i++
                ch = '\n'
            }
            if(ch === '\n' || ch === PARAGRAPH_SEPARATOR || ch === BEGINNING_OF_FRAME || ch === END_OF_FRAME || ch === '\r'){
                if(!editBlock){
                    editBlock = true
                    this.doc.beginEditBlock()
                }
                flush()
                this.doc.insertBlock(this.position, blockFormat, format)
                this.position++
            } else chunk += ch
        }
        flush()
        if(editBlock) this.doc.endEditBlock()
        this.anchor = this.position
        this.currentCharFormat = null
    }

    // QTextCursor::insertBlock(format, charFormat)
    insertBlock(blockFormat, charFormat){
        if(blockFormat === undefined) blockFormat = this.blockFormat()
        if(charFormat === undefined) charFormat = F.without(this.charFormat(), 'objectType')
        charFormat = F.without(charFormat, 'objectIndex')
        this.doc.beginEditBlock()
        this.removeSelection()
        this.doc.insertBlock(this.position, blockFormat, charFormat)
        this.doc.endEditBlock()
        this.position++
        this.anchor = this.position
        this.currentCharFormat = null
    }

    // the blocks of the selection (or the current one)
    selectedBlocks(){
        let first = this.doc.blockIndexAt(this.selectionStart())
        let last = this.doc.blockIndexAt(this.selectionEnd())
        let out = []
        for(let i = first; i <= last; i++) out.push(i)
        return out
    }

    // QTextCursor::setBlockFormat (SetFormat): the list of the old format goes unless the new one has it
    setBlockFormat(format){
        for(let b of this.selectedBlocks()) this.doc.setBlockFormat(b, format)
    }

    mergeBlockFormat(modifier){
        for(let b of this.selectedBlocks()) this.doc.setBlockFormat(b, F.merge(this.doc.blockFormat(b), modifier))
    }

    setBlockCharFormat(format){
        format = F.without(format, 'objectIndex')
        for(let b of this.selectedBlocks()) this.doc.setBlockCharFormat(b, format, false)
    }

    mergeBlockCharFormat(modifier){
        modifier = F.without(modifier, 'objectIndex')
        for(let b of this.selectedBlocks()) this.doc.setBlockCharFormat(b, modifier, true)
    }

    // QTextCursor::mergeCharFormat: the selection, or the format of the cursor without one
    mergeCharFormat(modifier){
        if(!this.hasSelection()){
            this.currentCharFormat = F.merge(this.charFormat(), modifier)
            return
        }
        let start = this.selectionStart()
        this.doc.setCharFormat(start, this.selectionEnd() - start, modifier, true)
    }

    setCharFormat(format){
        if(!this.hasSelection()){
            this.currentCharFormat = format
            return
        }
        let start = this.selectionStart()
        this.doc.setCharFormat(start, this.selectionEnd() - start, format, false)
    }

    // QTextCursor::insertImage: the object replacement character in the image format
    insertImage(format){
        this.insertText(OBJECT_REPLACEMENT, F.with_(format, { objectType: 'image' }))
    }

    // QTextCursor::createList: a list of the format with the current block as its first item
    createList(format){
        let list = this.doc.createListObject(format)
        this.mergeBlockFormat({ list })
        return list
    }

    // QTextCursor::insertTable: the table at the cursor, the cursor in its first cell
    insertTable(rows, columns, format){
        if(!rows || !columns) return null
        let pos = this.position
        let table = this.doc.insertTable(pos, rows, columns, format)
        this.position = this.anchor = pos + 1
        this.currentCharFormat = null
        return table
    }

    // QTextCursor::movePosition (QTextCursorPrivate::movePosition) n times; lines: the layout's line of a position
    // { start, end, up(pos) -> pos, down(pos) -> pos } (Up, Down, StartOfLine, EndOfLine), null: the block is the line.
    // Whether it moved every time
    movePosition(op, keepAnchor = false, n = 1, lines = null){
        let doc = this.doc
        let text = () => doc.rawText()
        let isWord = (c) => c !== undefined && /[\p{L}\p{N}\p{M}_]/u.test(c)
        for(let i = 0; i < n; i++){
            let pos = this.position
            let content = text()
            let b = doc.blockIndexAt(pos)
            let blockStart = doc.blockStart(b), blockEnd = blockStart + doc.blockLength(b) - 1
            let next = pos
            switch(op){
                case MOVE.NoMove: break
                case MOVE.Start: next = 0; break
                case MOVE.End: next = doc.length(); break
                case MOVE.StartOfBlock: next = blockStart; break
                case MOVE.EndOfBlock: next = blockEnd; break
                case MOVE.PreviousBlock:
                    if(b === 0) return false
                    next = doc.blockStart(b - 1)
                    break
                case MOVE.NextBlock:
                    if(b === doc.blocks.length - 1) return false
                    next = doc.blockStart(b + 1)
                    break
                case MOVE.PreviousCharacter: case MOVE.Left:
                    if(pos === 0) return false
                    next = pos - 1
                    if(next > 0 && content[next] >= '\udc00' && content[next] <= '\udfff' && content[next - 1] >= '\ud800' && content[next - 1] <= '\udbff') next--
                    break
                case MOVE.NextCharacter: case MOVE.Right:
                    if(pos >= doc.length()) return false
                    next = pos + 1
                    if(next < content.length && content[next] >= '\udc00' && content[next] <= '\udfff' && content[next - 1] >= '\ud800' && content[next - 1] <= '\udbff') next++
                    break
                case MOVE.StartOfWord:
                    while(next > blockStart && isWord(content[next - 1])) next--
                    break
                case MOVE.EndOfWord:
                    while(next < blockEnd && isWord(content[next])) next++
                    break
                case MOVE.PreviousWord: case MOVE.WordLeft:
                    if(pos === 0) return false
                    while(next > 0 && !isWord(content[next - 1])) next--
                    while(next > 0 && isWord(content[next - 1])) next--
                    break
                case MOVE.NextWord: case MOVE.WordRight:
                    if(pos >= doc.length()) return false
                    while(next < content.length && isWord(content[next])) next++
                    while(next < content.length && !isWord(content[next])) next++
                    break
                case MOVE.StartOfLine:
                    next = lines ? lines.start(pos) : blockStart
                    break
                case MOVE.EndOfLine:
                    next = lines ? lines.end(pos) : blockEnd
                    break
                case MOVE.Up: case MOVE.Down: {
                    let to = lines ? (op === MOVE.Up ? lines.up(pos) : lines.down(pos)) : (op === MOVE.Up ? (b > 0 ? doc.blockStart(b - 1) : -1) : (b < doc.blocks.length - 1 ? doc.blockStart(b + 1) : -1))
                    if(to < 0) return false
                    next = to
                    break
                }
                case MOVE.NextCell: case MOVE.PreviousCell: case MOVE.NextRow: case MOVE.PreviousRow: {
                    let at = doc.cellAt(pos)
                    if(!at) return false
                    let { grid, cell } = at
                    let target = null
                    if(op === MOVE.NextCell){
                        let index = grid.cells.indexOf(cell)
                        target = grid.cells[index + 1]
                    } else if(op === MOVE.PreviousCell){
                        let index = grid.cells.indexOf(cell)
                        target = grid.cells[index - 1]
                    } else if(op === MOVE.NextRow){
                        let row = cell.row + cell.rowSpan
                        for(let c = 0; c < grid.columns && !target; c++){
                            let candidate = grid.cellAt(row, c)
                            if(candidate && candidate.row === row) target = candidate
                        }
                    } else {
                        for(let c = grid.columns - 1; c >= 0 && !target; c--){
                            let candidate = grid.cellAt(cell.row - 1, c)
                            if(candidate) target = candidate
                        }
                    }
                    if(!target) return false
                    next = target.first
                    break
                }
                default: return false
            }
            if(next === pos && op !== MOVE.NoMove && op !== MOVE.StartOfBlock && op !== MOVE.EndOfBlock && op !== MOVE.StartOfLine
                && op !== MOVE.EndOfLine && op !== MOVE.StartOfWord && op !== MOVE.EndOfWord && op !== MOVE.Start && op !== MOVE.End) return false
            this.setPosition(next, keepAnchor)
        }
        return true
    }

    // QTextCursor::select(WordUnderCursor): the word the cursor is in or at the end of
    selectWordUnderCursor(){
        let content = this.doc.rawText()
        let isWord = (c) => c !== undefined && /[\p{L}\p{N}\p{M}_]/u.test(c)
        let start = this.position, end = this.position
        while(start > 0 && isWord(content[start - 1])) start--
        while(end < content.length && isWord(content[end])) end++
        this.setPosition(start)
        this.setPosition(end, true)
    }

    movePreviousCharacter(keepAnchor){
        this.setPosition(this.position - 1, keepAnchor)
    }

    moveNextCharacter(keepAnchor){
        this.setPosition(this.position + 1, keepAnchor)
    }
}

module.exports = { TextCursor, MOVE }
