const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { Layout, LayoutAttached, align, hints, solve, snapCells } = require('./Layout')

class LinearLayout extends Layout {
    $horizontal(){ return true }

    $minimum(horizontal){
        let items = this.$items()
        let main = horizontal === this.$horizontal()
        let values = items.map(item => hints(item, horizontal, this).min)
        if(!values.length) return 0
        return main ? values.reduce((a, b) => a + b, 0) + this.$v[LinearLayout.SPACING] * (values.length - 1) : Math.max(...values)
    }

    $arrange(){
        let horizontal = this.$horizontal()
        let items = this.$items()
        let spacing = this.$v[LinearLayout.SPACING]
        let mainHints = items.map(item => hints(item, horizontal, this))
        let crossHints = items.map(item => hints(item, !horizontal, this))

        let mainTotal = mainHints.reduce((sum, h) => sum + h.preferred, 0) + spacing * Math.max(0, items.length - 1)
        let crossTotal = crossHints.reduce((max, h) => Math.max(max, h.preferred), 0)
        this.$write(horizontal ? Item.IMPLICITWIDTH : Item.IMPLICITHEIGHT, mainTotal)
        this.$write(horizontal ? Item.IMPLICITHEIGHT : Item.IMPLICITWIDTH, crossTotal)

        let mainSize = this.$v[horizontal ? Item.WIDTH : Item.HEIGHT]
        let crossSize = this.$v[horizontal ? Item.HEIGHT : Item.WIDTH]
        let solved = solve(mainHints, mainSize - spacing * Math.max(0, items.length - 1))
        let snapped = snapCells(solved, spacing, mainSize, mainHints.map(h => h.min))
        let cells = snapped.sizes
        // a row laid out from the right (layoutDirection Qt.RightToLeft): the geometry is mirrored in the layout
        let mirrored = horizontal && this.$v[LinearLayout.LAYOUTDIRECTION] === 1

        let geometry = []
        for(let i = 0; i < items.length; i++){
            let pos = snapped.positions[i]
            let item = items[i]
            let attached = item.$v[Item.LAYOUT]
            let flags = attached ? attached.$v[LayoutAttached.ALIGNMENT] : 0
            let main = mainHints[i], cross = crossHints[i]
            let mainItem = main.fill ? Math.min(cells[i], main.max) : Math.min(main.preferred, cells[i])
            let crossItem = cross.fill ? Math.max(cross.min, Math.min(crossSize, cross.max)) : Math.min(cross.preferred, Math.max(crossSize, cross.preferred))
            let mainContent = mainItem - main.before - main.after
            let crossContent = crossItem - cross.before - cross.after
            let mainPos = align(pos, cells[i], mainItem, flags, horizontal) + main.before
            let crossPos = align(0, crossSize, crossItem, flags, !horizontal) + cross.before
            if(mirrored) mainPos = mainSize - mainPos - mainContent
            geometry.push(horizontal ? [item, mainPos, crossPos, mainContent, crossContent] : [item, crossPos, mainPos, crossContent, mainContent])
        }
        for(let i = geometry.length - 1; i >= 0; i--) this.$placeItem(...geometry[i])
    }
}

defineType(LinearLayout, {
    props: {
        spacing: { kind: 'real', initial: 5, effect: (layout) => layout.$invalidate() },
        layoutDirection: { kind: 'int', effect: (layout) => layout.$invalidate() },
        uniformCellSizes: 'bool',
    },
})

module.exports = { LinearLayout }
