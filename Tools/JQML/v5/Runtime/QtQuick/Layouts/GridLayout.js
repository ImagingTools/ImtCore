const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { Layout, LayoutAttached, align, hints, solve } = require('./Layout')

// ---- GridLayout ----------------------------------------------------------------------------------

class GridLayout extends Layout {
    // cell of each item: explicit Layout.row / Layout.column, otherwise auto-placed along `flow`
    $cells(items){
        let leftToRight = this.$v[GridLayout.FLOW] === 0
        let limit = leftToRight ? this.$v[GridLayout.COLUMNS] : this.$v[GridLayout.ROWS]
        let occupied = new Set()
        let row = 0, column = 0
        let result = []
        for(let item of items){
            let attached = item.$v[Item.LAYOUT]
            let r = attached ? attached.$v[LayoutAttached.ROW] : -1
            let c = attached ? attached.$v[LayoutAttached.COLUMN] : -1
            let rowSpan = Math.max(1, attached ? attached.$v[LayoutAttached.ROWSPAN] || 1 : 1)
            let columnSpan = Math.max(1, attached ? attached.$v[LayoutAttached.COLUMNSPAN] || 1 : 1)
            const free = (r0, c0) => {
                for(let i = 0; i < rowSpan; i++) for(let j = 0; j < columnSpan; j++) if(occupied.has((r0 + i) + ':' + (c0 + j))) return false
                return true
            }
            if(r < 0 || c < 0){
                while(!free(row, column) || (leftToRight && limit > 0 && column + columnSpan > limit && column > 0)){
                    if(leftToRight){
                        column++
                        if(limit > 0 && column >= limit){ column = 0; row++ }
                    } else {
                        row++
                        if(limit > 0 && row >= limit){ row = 0; column++ }
                    }
                }
                r = row
                c = column
            }
            for(let i = 0; i < rowSpan; i++) for(let j = 0; j < columnSpan; j++) occupied.add((r + i) + ':' + (c + j))
            result.push({ item, row: r, column: c, rowSpan, columnSpan })
        }
        return result
    }

    // QGridLayoutEngine: the lines from the items of one line, then the items that span several: what their lines lack
    // goes to the lines that fill (else to all of them), and an item that fills makes its lines fill when none does
    $axis(cells, horizontal, spacing){
        let count = 0
        let start = (cell) => (horizontal ? cell.column : cell.row)
        let span = (cell) => (horizontal ? cell.columnSpan : cell.rowSpan)
        for(let cell of cells) count = Math.max(count, start(cell) + span(cell))
        let lines = []
        for(let i = 0; i < count; i++) lines.push({ preferred: 0, min: 0, max: Infinity, fill: false, stretch: -1, used: false })
        for(let cell of cells){
            let h = horizontal ? cell.hx : cell.hy
            if(span(cell) > 1) continue
            let line = lines[start(cell)]
            line.preferred = Math.max(line.preferred, h.preferred)
            line.min = Math.max(line.min, h.min)
            if(h.fill) line.fill = true
            line.used = true
        }
        for(let cell of cells){
            if(span(cell) <= 1) continue
            let h = horizontal ? cell.hx : cell.hy
            let spanned = lines.slice(start(cell), start(cell) + span(cell))
            spanned.forEach(line => { line.used = true })
            if(h.fill && !spanned.some(line => line.fill)) spanned.forEach(line => { line.fill = true })
            let gaps = spacing * (spanned.length - 1)
            for(let key of ['min', 'preferred']){
                let lack = h[key] - gaps - spanned.reduce((t, line) => t + line[key], 0)
                if(lack <= 0) continue
                let takers = spanned.filter(line => line.fill)
                if(!takers.length) takers = spanned
                for(let line of takers) line[key] += lack / takers.length
                if(key === 'min') for(let line of spanned) line.preferred = Math.max(line.preferred, line.min)
            }
        }
        return lines
    }

    $arrange(){
        let items = this.$items()
        let cells = this.$cells(items)
        for(let cell of cells){
            cell.hx = hints(cell.item, true, this)
            cell.hy = hints(cell.item, false, this)
        }
        let columnSpacing = this.$v[GridLayout.COLUMNSPACING], rowSpacing = this.$v[GridLayout.ROWSPACING]
        let columns = this.$axis(cells, true, columnSpacing)
        let rows = this.$axis(cells, false, rowSpacing)
        let sum = (lines, spacing) => lines.reduce((total, line) => total + line.preferred, 0) + spacing * Math.max(0, lines.length - 1)
        this.$write(Item.IMPLICITWIDTH, sum(columns, columnSpacing))
        this.$write(Item.IMPLICITHEIGHT, sum(rows, rowSpacing))
        let widths = solve(columns, this.$v[Item.WIDTH] - columnSpacing * Math.max(0, columns.length - 1))
        let heights = solve(rows, this.$v[Item.HEIGHT] - rowSpacing * Math.max(0, rows.length - 1))
        let starts = (sizes, spacing) => {
            let result = [], pos = 0
            for(let size of sizes){ result.push(pos); pos += size + spacing }
            return result
        }
        let xs = starts(widths, columnSpacing), ys = starts(heights, rowSpacing)
        let geometry = []
        for(let cell of cells){
            let attached = cell.item.$v[Item.LAYOUT]
            let flags = attached ? attached.$v[LayoutAttached.ALIGNMENT] : 0
            // the cell of an item: its lines and the spacing between them
            let cw = widths.slice(cell.column, cell.column + cell.columnSpan).reduce((t, x) => t + x, 0) + columnSpacing * (cell.columnSpan - 1)
            let ch = heights.slice(cell.row, cell.row + cell.rowSpan).reduce((t, x) => t + x, 0) + rowSpacing * (cell.rowSpan - 1)
            let w = cell.hx.fill ? Math.min(cw, cell.hx.max) : Math.min(cell.hx.preferred, cw)
            let h = cell.hy.fill ? Math.min(ch, cell.hy.max) : Math.min(cell.hy.preferred, ch)
            geometry.push([cell.item,
                align(xs[cell.column], cw, w, flags, true) + cell.hx.before,
                align(ys[cell.row], ch, h, flags, false) + cell.hy.before,
                w - cell.hx.before - cell.hx.after,
                h - cell.hy.before - cell.hy.after])
        }
        for(let i = geometry.length - 1; i >= 0; i--) this.$placeItem(...geometry[i])
    }
}

defineType(GridLayout, {
    props: {
        columns: { kind: 'int', initial: -1, effect: (layout) => layout.$invalidate() },
        rows: { kind: 'int', initial: -1, effect: (layout) => layout.$invalidate() },
        flow: { kind: 'int', effect: (layout) => layout.$invalidate() },
        columnSpacing: { kind: 'real', initial: 5, effect: (layout) => layout.$invalidate() },
        rowSpacing: { kind: 'real', initial: 5, effect: (layout) => layout.$invalidate() },
        layoutDirection: 'int',
        uniformCellWidths: 'bool',
        uniformCellHeights: 'bool',
    },
})
GridLayout.LeftToRight = 0
GridLayout.TopToBottom = 1

module.exports = { GridLayout }
