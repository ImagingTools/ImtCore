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
        if(this.$f & engine.COMPLETED && !this.$arranging) engine.queuePolish(this)
    }

    $childGeometryChanged(){
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
    $place(item, x, y, width, height){
        item.$place(Item.X, x)
        item.$place(Item.Y, y)
        item.$write(Item.WIDTH, width)
        item.$write(Item.HEIGHT, height)
    }

    $polish(){
        this.$arranging = true
        try {
            this.$arrange()
        } finally {
            this.$arranging = false
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
        base.call(this, id, value)
        if((id === Item.WIDTH || id === Item.HEIGHT) && this.$f & engine.COMPLETED && !this.$arranging) engine.queuePolish(this)
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

module.exports = { LayoutAttached, hints, solve, Layout, align }
