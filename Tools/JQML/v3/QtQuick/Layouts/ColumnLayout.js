const Item = require("../Item")
const Repeater = require("../Repeater")
const Geometry = require("../../QtQml/Geometry")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Bool = require("../../QtQml/Bool")
const Signal = require("../../QtQml/Signal")
const QtEnums = require("../../Qt/enums")

class ColumnLayout extends Item {
    static LeftToRight = 0
    static RightToLeft = 1

    static meta = Object.assign({}, Item.meta, {
        layoutDirection: { type: Int, value: ColumnLayout.LeftToRight},
        spacing: { type: Real, value: 5},
        uniformCellSizes: { type: Bool, value: false },

        layoutDirectionChanged: {type:Signal, args:[]},
        spacingChanged: {type:Signal, args:[]},
        uniformCellSizesChanged: {type:Signal, args:[]},
    })

    
    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__DOM.classList.add('ColumnLayout')

        return obj
    }

    __updateProperties(){
        super.__updateProperties()

        this.__updateGeometry(true)
    }

    'SLOT_Component.completed'(){
        JQApplication.updateLater(this)
    }

    SLOT_spacingChanged(oldValue, newValue){
        this.__setDOMStyle({
            gap: newValue + 'px'
        })
        JQApplication.updateLater(this)
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

        const spacing = this.__proxy.spacing
        const hasW    = !!this.__self.width__prevent
        const hasH    = !!this.__self.height__prevent
        const layoutW = this.__proxy.width
        const layoutH = this.__proxy.height

        // ── collect visible children ──────────────────────────────────────
        const items = []
        for(const child of this.children){
            if(child instanceof Repeater || !child.visible || child.__destroying) continue
            items.push(child)
        }

        if(items.length === 0){
            Geometry.setAuto(this.__self, 'width',  0, this.__self.constructor.meta.width)
            Geometry.setAuto(this.__self, 'height', 0, this.__self.constructor.meta.height)
            this.__layouting = false
            return
        }

        // ── read Layout hints per child ───────────────────────────────────
        const hints = items.map(child => {
            const L  = child.Layout
            const lm = L.leftMargin   > 0 ? L.leftMargin   : L.margins
            const rm = L.rightMargin  > 0 ? L.rightMargin  : L.margins
            const tm = L.topMargin    > 0 ? L.topMargin    : L.margins
            const bm = L.bottomMargin > 0 ? L.bottomMargin : L.margins
            return {
                fillW:   !!L.fillWidth,
                fillH:   !!L.fillHeight,
                minW:    L.minimumWidth  || 0,
                maxW:    L.maximumWidth  < Infinity ? L.maximumWidth  : Infinity,
                minH:    L.minimumHeight || 0,
                maxH:    L.maximumHeight < Infinity ? L.maximumHeight : Infinity,
                prefW:   L.preferredWidth,
                prefH:   L.preferredHeight,
                vStr:    L.verticalStretchFactor > 0 ? L.verticalStretchFactor : 1,
                align:   L.alignment,
                lm, rm, tm, bm,
                naturalH: 0,
                cellH:    0,
                itemH:    0,
                naturalW: 0,
                itemW:    0,
                outX:     0,
                outY:     0,
            }
        })

        // ── PASS 1: compute everything in memory (no DOM changes) ─────────

        for(let i = 0; i < items.length; i++){
            const h = hints[i]
            let hh = h.prefH > 0 ? h.prefH : (items[i].height || 0)
            if(h.minH > 0)        hh = Math.max(hh, h.minH)
            if(h.maxH < Infinity) hh = Math.min(hh, h.maxH)
            h.naturalH = Math.max(0, hh)

            let w = h.prefW > 0 ? h.prefW : (items[i].width || 0)
            if(h.minW > 0)        w = Math.max(w, h.minW)
            if(h.maxW < Infinity) w = Math.min(w, h.maxW)
            h.naturalW = Math.max(0, w)
        }

        const fillIdx    = items.map((_, i) => i).filter(i => hints[i].fillH)
        const nonFillIdx = items.map((_, i) => i).filter(i => !hints[i].fillH)

        if(hasH && fillIdx.length === 0){
            const available = layoutH - (items.length - 1) * spacing
            const totalNat  = nonFillIdx.reduce((s, i) => s + hints[i].naturalH, 0)
            for(let i = 0; i < items.length; i++){
                hints[i].cellH = totalNat > 0 ? available * hints[i].naturalH / totalNat : hints[i].naturalH
                hints[i].itemH = hints[i].naturalH
            }
        } else if(hasH && fillIdx.length > 0){
            let fixedSum = nonFillIdx.reduce((s, i) => s + hints[i].naturalH + hints[i].tm + hints[i].bm, 0)
            fixedSum    += (items.length - 1) * spacing
            const fillMargSum  = fillIdx.reduce((s, i) => s + hints[i].tm + hints[i].bm, 0)
            const extra        = layoutH - fixedSum - fillMargSum
            const totalStretch = fillIdx.reduce((s, i) => s + hints[i].vStr, 0)
            for(const i of nonFillIdx){ hints[i].cellH = hints[i].naturalH; hints[i].itemH = hints[i].naturalH }
            for(const i of fillIdx){
                let hh = Math.max(0, extra * hints[i].vStr / totalStretch)
                if(hints[i].minH > 0)        hh = Math.max(hh, hints[i].minH)
                if(hints[i].maxH < Infinity) hh = Math.min(hh, hints[i].maxH)
                hints[i].cellH = Math.max(0, hh)
                hints[i].itemH = hints[i].cellH
            }
        } else {
            for(let i = 0; i < items.length; i++){
                hints[i].cellH = hints[i].naturalH
                hints[i].itemH = hints[i].naturalH
            }
        }

        const maxNatW = hints.reduce((m, h) => Math.max(m, h.naturalW + h.lm + h.rm), 0)
        const finalW  = hasW ? layoutW : maxNatW

        for(let i = 0; i < items.length; i++){
            const h = hints[i]
            h.itemW = h.fillW ? Math.max(0, finalW - h.lm - h.rm) : h.naturalW
        }

        let totalH = (items.length - 1) * spacing
        for(const h of hints) totalH += h.cellH + h.tm + h.bm
        const finalH = hasH ? layoutH : totalH

        // compute output positions for each child
        let y = 0
        for(let i = 0; i < items.length; i++){
            const h = hints[i]
            if     (h.align & QtEnums.AlignTop)    h.outY = y + h.tm
            else if(h.align & QtEnums.AlignBottom) h.outY = y + h.tm + (h.cellH - h.itemH)
            else                                   h.outY = y + h.tm + Math.round((h.cellH - h.itemH) / 2)

            if(h.fillW)                                h.outX = h.lm
            else if(h.align & QtEnums.AlignHCenter)    h.outX = Math.round((finalW - h.itemW) / 2)
            else if(h.align & QtEnums.AlignRight)      h.outX = finalW - h.itemW - h.rm
            else                                       h.outX = h.lm

            y += h.cellH + h.tm + h.bm + spacing
        }

        // ── PASS 2: set layout own size FIRST (matches Qt signal order) ───
        if(!hasW) Geometry.setAuto(this.__self, 'width',  finalW, this.__self.constructor.meta.width)
        if(!hasH) Geometry.setAuto(this.__self, 'height', finalH, this.__self.constructor.meta.height)

        // ── PASS 3: set children — fill items first (matches Qt signal order) ──
        const applyOrder = [...fillIdx, ...nonFillIdx]
        for(const i of applyOrder){
            const h     = hints[i]
            const child = items[i]

            if(h.outY !== child.y) child.y = h.outY
            if(h.outX !== child.x) child.x = h.outX
            if(h.itemW !== child.width)  child.width  = h.itemW
            if(h.itemH !== child.height) child.height = h.itemH
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



module.exports = ColumnLayout