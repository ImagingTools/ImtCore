// LayoutMirroring of QtQuick (QQuickLayoutMirroringAttached, QQuickItemPrivate::setImplicitLayoutMirror, Qt 6.12).
// enabled mirrors the layout of the item: its anchors (left and right swapped with their margins, the horizontal
// center offset negated), a Row / Grid / Flow and a RowLayout / GridLayout laid out from the other side, a Text aligned
// explicitly to the other side. childrenInherit passes the mirroring of the item to the items below that do not set
// their own; reading enabled gives the effective mirroring.
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { GroupObject } = require('../Core/QtObject')
const { Item } = require('./Item')

let used = false        // no LayoutMirroring in the application: nothing is mirrored (the fast path)

class LayoutMirroringAttached extends GroupObject {
    constructor(owner){
        super(owner)
        this.$enabledSet = false
        used = true
    }

    $write(id, value){
        let before = mirrorStates(this.$owner)
        if(id === LayoutMirroringAttached.ENABLED){
            // undefined resets it (QQuickLayoutMirroringAttached::resetEnabled): the item inherits again
            this.$enabledSet = value !== undefined
            super.$write(id, !!value)
        } else super.$write(id, value)
        mirroringChanged(this.$owner, before)
    }
}
defineType(LayoutMirroringAttached, {
    props: {
        enabled: { kind: 'bool', get(){ engine.track(this, LayoutMirroringAttached.SIGNAL_enabledChanged); return mirrorState(this.$owner).effective } },
        childrenInherit: 'bool',
    },
})

function attachedOf(item){
    let id = Item.$type.propByName.get('LayoutMirroring').id
    return item.$v[id] || null
}

// QQuickItemPrivate: { inherit (inheritMirrorFromParent), inherited (inheritedLayoutMirror), effective
// (effectiveLayoutMirror) } of an item, from the items above
function mirrorState(item){
    if(!used || !item) return { inherit: false, inherited: false, effective: false }
    let parent = item.$parent instanceof Item ? item.$parent : null
    let ps = parent ? mirrorState(parent) : { inherit: false, inherited: false }
    let att = attachedOf(item)
    let explicit = !!(att && att.$enabledSet)
    let childrenInherit = !!(att && att.$v[LayoutMirroringAttached.CHILDRENINHERIT])
    let inherit = ps.inherit || childrenInherit
    let mirror = ps.inherited
    if(explicit && childrenInherit) mirror = att.$v[LayoutMirroringAttached.ENABLED]
    let inherited = inherit ? mirror : false
    let effective = explicit ? att.$v[LayoutMirroringAttached.ENABLED] : inherited
    return { inherit, inherited, effective }
}

function mirrored(item){ return used ? mirrorState(item).effective : false }

// the effective mirroring of the item and the items below it, before a change
function mirrorStates(root){
    let states = new Map()
    let visit = (item) => {
        states.set(item, mirrored(item))
        for(let child of item.$children || []) if(child instanceof Item) visit(child)
    }
    if(root) visit(root)
    return states
}

// the items whose mirroring changed lay out again (their anchors, a positioner, a Text)
function mirroringChanged(root, before){
    for(let [item, was] of before){
        let now = mirrored(item)
        if(now === was) continue
        let att = attachedOf(item)
        if(att) engine.emit(att, LayoutMirroringAttached.SIGNAL_enabledChanged)
        if(item.$mirrorChanged) item.$mirrorChanged()
        let anchors = item.$v[Item.ANCHORS]
        if(anchors && anchors.$horizontal) require('./Item').updateHorizontalAnchors(anchors)
    }
}

{
    const prop = Item.$type.propByName.get('LayoutMirroring')
    prop.group = LayoutMirroringAttached
    Object.defineProperty(Item.prototype, 'LayoutMirroring', {
        get(){ return this.$v[prop.id] || (this.$v[prop.id] = new LayoutMirroringAttached(this)) },
        configurable: true,
    })
}

module.exports = { LayoutMirroringAttached, mirrored }
