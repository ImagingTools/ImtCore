const engine = require('../../Core/engine')
const { GroupObject } = require('../../Core/QtObject')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')

const AlignLeft = 0x1, AlignRight = 0x2, AlignHCenter = 0x4
const AlignTop = 0x20, AlignBottom = 0x40, AlignVCenter = 0x80

// ---- attached Layout.* ----------------------------------------------------------------------

const MARGIN_BITS = { leftMargin: 1, topMargin: 2, rightMargin: 4, bottomMargin: 8 }

class LayoutAttached extends GroupObject {
    constructor(owner){
        super(owner)
        this.$explicit = 0     // margins and fill set explicitly
    }

    $assign(id, value){
        let name = LayoutAttached.$type.props[id].name
        ;(this.$userSet || (this.$userSet = new Set())).add(name)
        if(MARGIN_BITS[name]) this.$explicit |= MARGIN_BITS[name]
        if(name === 'fillWidth') this.$explicit |= 16
        if(name === 'fillHeight') this.$explicit |= 32
        super.$assign(id, value)
    }

    $bind(id, fn, scope, self){
        let name = LayoutAttached.$type.props[id].name
        ;(this.$userSet || (this.$userSet = new Set())).add(name)
        if(MARGIN_BITS[name]) this.$explicit |= MARGIN_BITS[name]
        if(name === 'fillWidth') this.$explicit |= 16
        if(name === 'fillHeight') this.$explicit |= 32
        return super.$bind(id, fn, scope, self)
    }
}

const relayoutOwner = (attached) => {
    let parent = attached.$owner && attached.$owner.$parent
    if(parent instanceof Layout) parent.$invalidate()
}

defineType(LayoutAttached, {
    props: {
        minimumWidth: { kind: 'real', effect: relayoutOwner },
        minimumHeight: { kind: 'real', effect: relayoutOwner },
        preferredWidth: { kind: 'real', initial: -1, effect: relayoutOwner },
        preferredHeight: { kind: 'real', initial: -1, effect: relayoutOwner },
        maximumWidth: { kind: 'real', initial: Infinity, effect: relayoutOwner },
        maximumHeight: { kind: 'real', initial: Infinity, effect: relayoutOwner },
        fillWidth: { kind: 'bool', effect: relayoutOwner },
        fillHeight: { kind: 'bool', effect: relayoutOwner },
        alignment: { kind: 'int', effect: relayoutOwner },
        margins: { kind: 'real', effect: relayoutOwner },
        leftMargin: { kind: 'real', effect: relayoutOwner },
        topMargin: { kind: 'real', effect: relayoutOwner },
        rightMargin: { kind: 'real', effect: relayoutOwner },
        bottomMargin: { kind: 'real', effect: relayoutOwner },
        row: { kind: 'int', initial: -1, effect: relayoutOwner },
        column: { kind: 'int', initial: -1, effect: relayoutOwner },
        rowSpan: { kind: 'int', initial: 1, effect: relayoutOwner },
        columnSpan: { kind: 'int', initial: 1, effect: relayoutOwner },
        horizontalStretchFactor: { kind: 'real', initial: -1, effect: relayoutOwner },
        verticalStretchFactor: { kind: 'real', initial: -1, effect: relayoutOwner },
    },
})

// Size hints of one child along one axis
function hints(item, horizontal, parentLayout){
    let attached = item.$v[Item.LAYOUT]
    let v = attached ? attached.$v : null
    let P = LayoutAttached
    let margin = (name) => {
        if(!attached) return 0
        return attached.$explicit & MARGIN_BITS[name] ? v[P[name.toUpperCase()]] : v[P.MARGINS]
    }
    let before = horizontal ? margin('leftMargin') : margin('topMargin')
    let after = horizontal ? margin('rightMargin') : margin('bottomMargin')
    let implicit = item.$v[horizontal ? Item.IMPLICITWIDTH : Item.IMPLICITHEIGHT]
    let size = item.$v[horizontal ? Item.WIDTH : Item.HEIGHT]
    let preferred = v ? v[horizontal ? P.PREFERREDWIDTH : P.PREFERREDHEIGHT] : -1
    if(preferred < 0) preferred = item instanceof Layout ? implicit : (implicit > 0 ? implicit : size)
    let min = v ? v[horizontal ? P.MINIMUMWIDTH : P.MINIMUMHEIGHT] : 0
    let max = v ? v[horizontal ? P.MAXIMUMWIDTH : P.MAXIMUMHEIGHT] : Infinity
    if(item instanceof Layout && !(v && v[horizontal ? P.MINIMUMWIDTH : P.MINIMUMHEIGHT])) min = item.$minimum(horizontal)
    let fillExplicit = attached && attached.$explicit & (horizontal ? 16 : 32)
    let fill = fillExplicit ? !!v[horizontal ? P.FILLWIDTH : P.FILLHEIGHT] : (item instanceof Layout || (parentLayout instanceof require('./StackLayout').StackLayout))
    let stretch = v ? v[horizontal ? P.HORIZONTALSTRETCHFACTOR : P.VERTICALSTRETCHFACTOR] : -1
    preferred = Math.max(min, Math.min(max, preferred))
    // snapped to the pixel grid: the sizes of an item are whole pixels, up (QQuickLayout, QGridLayoutItem::box)
    preferred = Math.ceil(preferred - 1e-9)
    min = Math.ceil(min - 1e-9)
    return { preferred: preferred + before + after, min: min + before + after, max: max + before + after, fill, stretch, before, after }
}

// Cell sizes along one axis for `available` space
function solve(cells, available){
    let total = cells.reduce((sum, cell) => sum + cell.preferred, 0)
    let sizes = cells.map(cell => cell.preferred)
    let extra = available - total
    if(extra > 0){
        let growing = cells.map((cell, i) => i).filter(i => cells[i].fill)
        if(growing.length){
            // spare space to fill cells, by stretch factor when set, else by preferred size
            let weight = (i) => cells[i].stretch > 0 ? cells[i].stretch : Math.max(cells[i].preferred, 0)
            while(extra > 0.0001 && growing.length){
                let totalWeight = growing.reduce((sum, i) => sum + weight(i), 0)
                let capped = []
                let used = 0
                for(let i of growing){
                    let share = totalWeight > 0 ? extra * weight(i) / totalWeight : extra / growing.length
                    let room = cells[i].max - sizes[i]
                    if(share >= room){
                        sizes[i] += room
                        used += room
                        capped.push(i)
                    }
                }
                if(capped.length){
                    extra -= used
                    growing = growing.filter(i => capped.indexOf(i) < 0)
                    continue
                }
                for(let i of growing) sizes[i] += totalWeight > 0 ? extra * weight(i) / totalWeight : extra / growing.length
                extra = 0
            }
        } else if(total > 0){
            for(let i = 0; i < cells.length; i++) sizes[i] += extra * cells[i].preferred / total
        } else if(cells.length){
            for(let i = 0; i < cells.length; i++) sizes[i] += extra / cells.length
        }
    } else if(extra < 0){
        // shrink fill cells toward their minimum
        let shrinking = cells.map((cell, i) => i).filter(i => cells[i].fill && sizes[i] > cells[i].min)
        let deficit = -extra
        while(deficit > 0.0001 && shrinking.length){
            let room = shrinking.reduce((sum, i) => sum + (sizes[i] - cells[i].min), 0)
            let take = Math.min(deficit, room)
            for(let i of shrinking) sizes[i] -= take * (sizes[i] - cells[i].min) / room
            deficit -= take
            shrinking = []
        }
    }
    return sizes
}

// QGridLayoutRowData::calculateGeometries with snapToPixelGrid: the cells start at whole pixels (the difference goes to
// the cell before), the last one ends at the size, each at least its minimum. { positions, sizes } of the cells
function snapCells(sizes, spacing, target, minimums){
    let n = sizes.length
    let positions = [], out = sizes.slice()
    let at = 0
    for(let i = 0; i < n; i++){
        positions.push(at)
        at += sizes[i] + spacing
    }
    if(!n) return { positions, sizes: out }
    for(let i = 0; i < n; i++){
        let old = positions[i]
        positions[i] = qround(old)
        let delta = positions[i] - old
        out[i] -= delta
        if(i > 0) out[i - 1] += delta
    }
    out[n - 1] = target - positions[n - 1]
    for(let i = 0; i < n; i++) out[i] = Math.max(minimums[i], qround(out[i]))
    return { positions, sizes: out }
}
// qRound
const qround = (d) => (d >= 0 ? Math.floor(d + 0.5) : Math.ceil(d - 0.5))

// ---- base -------------------------------------------------------------------------------------

class Layout extends Item {
    constructor(parent = null){
        super(parent)
        this.$arranging = false
        engine.scheduleComponentComplete(this)
    }

    $componentComplete(){
        super.$componentComplete()
        this.$polish()
    }

    $invalidate(){
        // a change while the layout arranges (a Text that wraps at its new width: a new implicit height) arranges again
        if(this.$arranging) this.$again = true
        else if(this.$f & engine.COMPLETED) engine.queuePolish(this)
    }

    $childGeometryChanged(){
        this.$invalidate()
    }

    $childImplicitChanged(){
        this.$invalidate()
    }

    $items(){
        let result = []
        for(let child of this.$children || []){
            if(child instanceof Item && child.$ownVisible && child !== this.$content) result.push(child)
        }
        return result
    }

    $minimum(horizontal){ return 0 }

    // position first, then size, like QQuickLayout::rearrange applying item geometry
    // (not $place: that is Item.$place(id, value), a layout in a layout is placed by it)
    $placeItem(item, x, y, width, height){
        item.$place(Item.X, x)
        item.$place(Item.Y, y)
        // QQuickItem::setWidth / setHeight of the layout: the size is valid (a Text wraps at it)
        item.$explicit |= 3
        item.$write(Item.WIDTH, width)
        item.$write(Item.HEIGHT, height)
    }

    $polish(){
        this.$arranging = true
        try {
            for(let pass = 0; pass < 3; pass++){
                this.$again = false
                this.$arrange()
                if(!this.$again) break
            }
        } finally {
            this.$arranging = false
            this.$again = false
        }
    }
}

defineType(Layout)

// alignment inside a cell along one axis
function align(start, cell, size, flags, horizontal){
    if(horizontal){
        if(flags & AlignRight) return start + cell - size
        if(flags & AlignHCenter) return start + (cell - size) / 2
        return start
    }
    if(flags & AlignTop) return start
    if(flags & AlignBottom) return start + cell - size
    return start + (cell - size) / 2
}

// layouts re-arrange when their own size changes
for(let Class of [Layout]){
    let base = Class.prototype.$write
    Class.prototype.$write = function(id, value){
        let old = id === Item.WIDTH || id === Item.HEIGHT ? this.$v[id] : undefined
        base.call(this, id, value)
        if(id !== Item.WIDTH && id !== Item.HEIGHT) return
        // a size that follows the implicit one changes while the layout arranges: it arranges again in the new size
        if(this.$arranging){
            if(this.$v[id] !== old) this.$again = true
        } else if(this.$f & engine.COMPLETED) engine.queuePolish(this)
    }
}

// The Layout attached group property of every Item, wired here because it needs LayoutAttached
{
    let prop = Item.$type.propByName.get('Layout')
    prop.group = LayoutAttached
    let id = prop.id
    Object.defineProperty(Item.prototype, 'Layout', {
        get(){ return this.$v[id] || (this.$v[id] = new LayoutAttached(this)) },
        configurable: true,
    })
}

module.exports = { LayoutAttached, hints, solve, snapCells, Layout, align }
