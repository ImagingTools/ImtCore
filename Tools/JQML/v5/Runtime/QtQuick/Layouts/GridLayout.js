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
            if(r < 0 || c < 0){
                while(occupied.has(row + ':' + column)){
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
            occupied.add(r + ':' + c)
            result.push({ item, row: r, column: c })
        }
        return result
    }

    $axis(cells, horizontal){
        let count = 0
        for(let cell of cells) count = Math.max(count, (horizontal ? cell.column : cell.row) + 1)
        let lines = []
        for(let i = 0; i < count; i++) lines.push({ preferred: 0, min: 0, max: Infinity, fill: false, stretch: -1, used: false })
        for(let cell of cells){
            let h = horizontal ? cell.hx : cell.hy
            let line = lines[horizontal ? cell.column : cell.row]
            line.preferred = Math.max(line.preferred, h.preferred)
            line.min = Math.max(line.min, h.min)
            if(h.fill) line.fill = true
            line.used = true
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
        let columns = this.$axis(cells, true)
        let rows = this.$axis(cells, false)
        let columnSpacing = this.$v[GridLayout.COLUMNSPACING], rowSpacing = this.$v[GridLayout.ROWSPACING]
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
            let cw = widths[cell.column], ch = heights[cell.row]
            let w = cell.hx.fill ? Math.min(cw, cell.hx.max) : Math.min(cell.hx.preferred, cw)
            let h = cell.hy.fill ? Math.min(ch, cell.hy.max) : Math.min(cell.hy.preferred, ch)
            geometry.push([cell.item,
                align(xs[cell.column], cw, w, flags, true) + cell.hx.before,
                align(ys[cell.row], ch, h, flags, false) + cell.hy.before,
                w - cell.hx.before - cell.hx.after,
                h - cell.hy.before - cell.hy.after])
        }
        for(let i = geometry.length - 1; i >= 0; i--) this.$place(...geometry[i])
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
