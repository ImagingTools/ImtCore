const Item = require("../Item")
const Repeater = require("../Repeater")
const Geometry = require("../../QtQml/Geometry")
const Int = require("../../QtQml/Int")
const Signal = require("../../QtQml/Signal")

class StackLayout extends Item {

    static meta = Object.assign({}, Item.meta, {
        currentIndex: { type: Int, value: 0 },
        count:        { type: Int, value: 0, modifiers: { readonly: false } },

        currentIndexChanged: { type: Signal, args: [] },
        countChanged:        { type: Signal, args: [] },
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__DOM.classList.add('StackLayout')
        return obj
    }

    __updateProperties(){
        super.__updateProperties()
        this.__updateGeometry(true)
    }

    'SLOT_Component.completed'(){
        JQApplication.updateLater(this)
    }

    SLOT_currentIndexChanged(){
        // Call with completed=true directly — __completed may still be false
        // if the user sets currentIndex from a parent's Component.onCompleted
        // (parent fires before children __complete())
        this.__updateGeometry(true)
    }

    SLOT_visibleChanged(oldValue, newValue){
        JQApplication.beginUpdate()
        super.SLOT_visibleChanged(oldValue, newValue)
        JQApplication.endUpdate()
    }

    __updateGeometry(completed = false){
        if(!completed || !this.__proxy.visible) return
        if(this.__layouting) return
        this.__layouting = true

        const hasW = !!this.__self.width__prevent
        const hasH = !!this.__self.height__prevent
        const layoutW = this.__proxy.width
        const layoutH = this.__proxy.height

        // collect visible items (excluding Repeaters)
        const items = []
        for(const child of this.children){
            if(child instanceof Repeater || child.__destroying) continue
            items.push(child)
        }

        // update count
        const count = items.length
        if(this.__proxy.count !== count) this.__proxy.count = count

        if(count === 0){
            Geometry.setAuto(this.__self, 'width',  0, this.__self.constructor.meta.width)
            Geometry.setAuto(this.__self, 'height', 0, this.__self.constructor.meta.height)
            this.__layouting = false
            return
        }

        const currentIndex = this.__proxy.currentIndex

        // ── Pass 1: compute natural sizes to determine layout implicit size ──
        let natW = 0, natH = 0
        for(let i = 0; i < items.length; i++){
            const child = items[i]
            const L = child.Layout
            const pw = L.preferredWidth  > 0 ? L.preferredWidth  : child.width  || 0
            const ph = L.preferredHeight > 0 ? L.preferredHeight : child.height || 0
            const minW = L.minimumWidth  > 0 ? L.minimumWidth  : 0
            const minH = L.minimumHeight > 0 ? L.minimumHeight : 0
            const w = Math.max(pw, minW)
            const h = Math.max(ph, minH)
            natW = Math.max(natW, w)
            natH = Math.max(natH, h)
        }

        const finalW = hasW ? layoutW : natW
        const finalH = hasH ? layoutH : natH

        // ── Pass 2: set layout size ──
        if(!hasW) Geometry.setAuto(this.__self, 'width',  finalW, this.__self.constructor.meta.width)
        if(!hasH) Geometry.setAuto(this.__self, 'height', finalH, this.__self.constructor.meta.height)

        // ── Pass 3: show only currentIndex; only the current item fills layout ──
        for(let i = 0; i < items.length; i++){
            const child   = items[i]
            const current = (i === currentIndex)

            if(current){
                // current item fills the layout
                if(child.x !== 0) child.x = 0
                if(child.y !== 0) child.y = 0
                if(child.width  !== finalW) child.width  = finalW
                if(child.height !== finalH) child.height = finalH
            }
            // all non-current items are hidden; they keep their natural sizes
            if(child.visible !== current) child.visible = current
        }

        this.__layouting = false
    }

    __endUpdate(){
        this.__updateGeometry(this.__completed)
        super.__endUpdate()
    }

    __removeChild(child){
        super.__removeChild(child)
        JQApplication.updateLater(this)
    }

    __addChild(child){
        super.__addChild(child)
        JQApplication.updateLater(this)
    }
}

module.exports = StackLayout
