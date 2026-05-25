const Item = require("../Item")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class MenuBar extends Item {
    static meta = Object.assign({}, Item.meta, {
        contentHeight: { type: Real, value: 0 },
        contentWidth:  { type: Real, value: 0 },
        delegate:      { type: Var },
        menus:         { type: Var, value: [] },

        contentHeightChanged: { type: Signal, args: [] },
        contentWidthChanged:  { type: Signal, args: [] },
        delegateChanged:      { type: Signal, args: [] },
        menusChanged:         { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}) {
        let obj = super.create(parent, properties)
        let dom = obj.__getDOM()
        dom.classList.add('MenuBar')
        obj.__setDOMStyle({
            display:        'flex',
            flexDirection:  'row',
            alignItems:     'center',
            boxSizing:      'border-box',
            position:       'relative',
        })
        return obj
    }

    // ── Layout update ────────────────────────────────────────────────────────

    // Recalculate implicit dimensions when children change
    __updateGeometry() {
        let w = 0
        let h = 0
        for (let child of this.children) {
            if (!child.visible) continue
            w += (child.width  || 0)
            h  = Math.max(h, child.height || 0)
        }
        this.implicitWidth  = w
        this.implicitHeight = h
        this.contentWidth   = w
        this.contentHeight  = h
    }

    SLOT_childrenChanged(oldValue, newValue) {
        super.SLOT_childrenChanged && super.SLOT_childrenChanged(oldValue, newValue)
        this.__updateGeometry()
    }
}



module.exports = MenuBar
