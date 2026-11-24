// Grid (QQuickGrid, Qt 6): its children in rows and columns; a column is as wide as its widest child, a row as tall as
// its tallest; columns 4 when neither rows nor columns is set, else the other one from the count; flow LeftToRight (by
// rows) or TopToBottom (by columns); rowSpacing / columnSpacing (spacing unless set); the item alignment in its cell
const engine = require('../Core/engine')
const { defineType, makeReadonly } = require('../Core/type')
const { Item } = require('./Item')
const { Positioner, relayout } = require('./Positioner')

const LeftToRight = 0, TopToBottom = 1
const AlignLeft = 1, AlignRight = 2, AlignHCenter = 4, AlignTop = 32, AlignBottom = 64, AlignVCenter = 128

class Grid extends Positioner {
    $polish(){
        let children = this.$visibleChildren()
        let n = children.length
        let v = this.$v
        let rows = v[Grid.ROWS], columns = v[Grid.COLUMNS]
        if(rows <= 0 && columns <= 0) columns = 4
        if(columns <= 0) columns = Math.max(1, Math.ceil(n / rows))
        if(rows <= 0) rows = Math.max(1, Math.ceil(n / columns))
        let topToBottom = v[Grid.FLOW] === TopToBottom
        let cell = (i) => (topToBottom ? { r: i % rows, c: Math.floor(i / rows) } : { r: Math.floor(i / columns), c: i % columns })
        let widths = new Array(columns).fill(0), heights = new Array(rows).fill(0)
        children.forEach((child, i) => {
            let { r, c } = cell(i)
            if(r >= rows || c >= columns) return
            widths[c] = Math.max(widths[c], child.$v[Item.WIDTH])
            heights[r] = Math.max(heights[r], child.$v[Item.HEIGHT])
        })
        let spacing = v[Positioner.SPACING]
        let columnSpacing = this.$columnSpacingSet ? v[Grid.COLUMNSPACING] : spacing
        let rowSpacing = this.$rowSpacingSet ? v[Grid.ROWSPACING] : spacing
        let left = this.$padding('leftPadding'), top = this.$padding('topPadding')
        let xs = [], ys = []
        let x = left
        for(let c = 0; c < columns; c++){ xs.push(x); x += widths[c] + (c < columns - 1 && widths[c] ? columnSpacing : 0) }
        let y = top
        for(let r = 0; r < rows; r++){ ys.push(y); y += heights[r] + (r < rows - 1 && heights[r] ? rowSpacing : 0) }
        let h = v[Grid.HORIZONTALITEMALIGNMENT], va = v[Grid.VERTICALITEMALIGNMENT]
        children.forEach((child, i) => {
            let { r, c } = cell(i)
            if(r >= rows || c >= columns) return
            let cx = xs[c], cy = ys[r]
            if(h & AlignRight) cx += widths[c] - child.$v[Item.WIDTH]
            else if(h & AlignHCenter) cx += (widths[c] - child.$v[Item.WIDTH]) / 2
            if(va & AlignBottom) cy += heights[r] - child.$v[Item.HEIGHT]
            else if(va & AlignVCenter) cy += (heights[r] - child.$v[Item.HEIGHT]) / 2
            child.$place(Item.X, cx)
            child.$place(Item.Y, cy)
        })
        let width = widths.reduce((s, w) => s + w, 0) + Math.max(0, widths.filter(w => w).length - 1) * columnSpacing
        let height = heights.reduce((s, w) => s + w, 0) + Math.max(0, heights.filter(w => w).length - 1) * rowSpacing
        this.$write(Item.IMPLICITWIDTH, width + left + this.$padding('rightPadding'))
        this.$write(Item.IMPLICITHEIGHT, height + top + this.$padding('bottomPadding'))
        this.$mirror(this.$visibleChildren())
        engine.emitArgs(this, Positioner.SIGNAL_positioningComplete, [])
    }
    $write(id, value){
        if(id === Grid.ROWSPACING) this.$rowSpacingSet = value !== undefined
        if(id === Grid.COLUMNSPACING) this.$columnSpacingSet = value !== undefined
        super.$write(id, value)
    }
}
defineType(Grid, {
    props: {
        rows: { kind: 'int', effect: relayout }, columns: { kind: 'int', effect: relayout },
        rowSpacing: { kind: 'real', effect: relayout }, columnSpacing: { kind: 'real', effect: relayout },
        flow: { kind: 'int', effect: relayout },
        layoutDirection: { kind: 'int', effect: (grid) => { engine.emit(grid, Grid.SIGNAL_effectiveLayoutDirectionChanged); relayout(grid) } },
        effectiveLayoutDirection: { kind: 'int', readonly: true },
        horizontalItemAlignment: { kind: 'int', initial: AlignLeft, effect: relayout },
        effectiveHorizontalItemAlignment: { kind: 'int', readonly: true, initial: AlignLeft },
        verticalItemAlignment: { kind: 'int', initial: AlignTop, effect: relayout },
    },
})
require('./Positioner').effectiveDirection(Grid)
Object.assign(Grid, { LeftToRight, TopToBottom, AlignLeft, AlignRight, AlignHCenter, AlignTop, AlignBottom, AlignVCenter })
makeReadonly(Grid, 'implicitWidth')
makeReadonly(Grid, 'implicitHeight')

module.exports = { Grid }
