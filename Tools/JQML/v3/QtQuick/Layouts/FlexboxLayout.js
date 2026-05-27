const Item     = require("../Item")
const Repeater = require("../Repeater")
const Geometry = require("../../QtQml/Geometry")
const Real     = require("../../QtQml/Real")
const Int      = require("../../QtQml/Int")
const Signal   = require("../../QtQml/Signal")

// ── FlexboxLayout ─────────────────────────────────────────────────────────────
// Implements CSS Flexbox semantics in pure JS (no yoga dependency).
// Supports:
//   direction       Row | RowReversed | Column | ColumnReversed
//   wrap            NoWrap | Wrap | WrapReverse
//   justifyContent  JustifyStart | JustifyCenter | JustifyEnd |
//                   JustifySpaceBetween | JustifySpaceAround | JustifySpaceEvenly
//   alignItems      AlignStart | AlignCenter | AlignEnd | AlignStretch
//   alignContent    AlignStart | AlignCenter | AlignEnd | AlignStretch |
//                   AlignSpaceBetween | AlignSpaceAround | AlignSpaceEvenly
//   gap / rowGap / columnGap
//   Per-item: FlexboxLayout.alignSelf (via attached property group)
//   Per-item: Layout.fillWidth / fillHeight / preferredWidth / preferredHeight /
//             minimumWidth / minimumHeight / maximumWidth / maximumHeight / margins
// ─────────────────────────────────────────────────────────────────────────────

class FlexboxLayout extends Item {

    // ── direction ──
    static Row            = 0
    static RowReversed    = 1
    static Column         = 2
    static ColumnReversed = 3

    // ── wrap ──
    static NoWrap      = 0
    static Wrap        = 1
    static WrapReverse = 2

    // ── justify ──
    static JustifyStart       = 0
    static JustifyCenter      = 1
    static JustifyEnd         = 2
    static JustifySpaceBetween = 3
    static JustifySpaceAround  = 4
    static JustifySpaceEvenly  = 5

    // ── align (items / content / self) ──
    static AlignAuto         = 0
    static AlignStart        = 1
    static AlignCenter       = 2
    static AlignEnd          = 3
    static AlignStretch      = 4
    static AlignSpaceBetween = 5
    static AlignSpaceAround  = 6
    static AlignSpaceEvenly  = 7

    static meta = Object.assign({}, Item.meta, {
        direction:      { type: Int,  value: FlexboxLayout.Row },
        wrap:           { type: Int,  value: FlexboxLayout.NoWrap },
        justifyContent: { type: Int,  value: FlexboxLayout.JustifyStart },
        alignItems:     { type: Int,  value: FlexboxLayout.AlignStart },
        alignContent:   { type: Int,  value: FlexboxLayout.AlignStart },
        gap:            { type: Real, value: 0 },
        rowGap:         { type: Real, value: -1 },   // -1 = use gap
        columnGap:      { type: Real, value: -1 },   // -1 = use gap

        directionChanged:      { type: Signal, args: [] },
        wrapChanged:           { type: Signal, args: [] },
        justifyContentChanged: { type: Signal, args: [] },
        alignItemsChanged:     { type: Signal, args: [] },
        alignContentChanged:   { type: Signal, args: [] },
        gapChanged:            { type: Signal, args: [] },
        rowGapChanged:         { type: Signal, args: [] },
        columnGapChanged:      { type: Signal, args: [] },
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__DOM.classList.add('FlexboxLayout')
        return obj
    }

    // ── lifecycle ─────────────────────────────────────────────────────────────

    __updateProperties(){
        super.__updateProperties()
        this.__updateGeometry(true)
    }

    'SLOT_Component.completed'(){
        JQApplication.updateLater(this)
    }

    SLOT_directionChanged()      { JQApplication.updateLater(this) }
    SLOT_wrapChanged()           { JQApplication.updateLater(this) }
    SLOT_justifyContentChanged() { JQApplication.updateLater(this) }
    SLOT_alignItemsChanged()     { JQApplication.updateLater(this) }
    SLOT_alignContentChanged()   { JQApplication.updateLater(this) }
    SLOT_gapChanged()            { JQApplication.updateLater(this) }
    SLOT_rowGapChanged()         { JQApplication.updateLater(this) }
    SLOT_columnGapChanged()      { JQApplication.updateLater(this) }

    SLOT_visibleChanged(oldValue, newValue){
        JQApplication.beginUpdate()
        super.SLOT_visibleChanged(oldValue, newValue)
        JQApplication.endUpdate()
    }

    __endUpdate(){
        this.__updateGeometry(this.__completed)
        super.__endUpdate()
    }

    __removeChild(child){ super.__removeChild(child); JQApplication.updateLater(this) }
    __addChild(child)   { super.__addChild(child);    JQApplication.updateLater(this) }

    // ── layout algorithm ──────────────────────────────────────────────────────

    __updateGeometry(completed = false){
        if(!completed || !this.__proxy.visible) return
        if(this.__layouting) return
        this.__layouting = true

        const dir    = this.__proxy.direction
        const wrapM  = this.__proxy.wrap
        const jc     = this.__proxy.justifyContent
        const ai     = this.__proxy.alignItems
        const ac     = this.__proxy.alignContent
        const gap    = this.__proxy.gap
        const rg     = this.__proxy.rowGap    >= 0 ? this.__proxy.rowGap    : gap
        const cg     = this.__proxy.columnGap >= 0 ? this.__proxy.columnGap : gap

        const isRow     = dir === FlexboxLayout.Row     || dir === FlexboxLayout.RowReversed
        const reversed  = dir === FlexboxLayout.RowReversed || dir === FlexboxLayout.ColumnReversed
        const mainGap   = isRow ? cg : rg   // gap along main axis
        const crossGap  = isRow ? rg : cg   // gap along cross axis

        const hasW = !!this.__self.width__prevent
        const hasH = !!this.__self.height__prevent
        const containerMain  = isRow ? this.__proxy.width  : this.__proxy.height
        const containerCross = isRow ? this.__proxy.height : this.__proxy.width

        // ── collect items ──
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

        // ── read hints ──
        const hints = items.map(child => {
            const L  = child.Layout
            const lm = L.leftMargin   > 0 ? L.leftMargin   : L.margins
            const rm = L.rightMargin  > 0 ? L.rightMargin  : L.margins
            const tm = L.topMargin    > 0 ? L.topMargin    : L.margins
            const bm = L.bottomMargin > 0 ? L.bottomMargin : L.margins

            let natW = L.preferredWidth  > 0 ? L.preferredWidth  : (child.width  || 0)
            let natH = L.preferredHeight > 0 ? L.preferredHeight : (child.height || 0)
            if(L.minimumWidth  > 0) natW = Math.max(natW, L.minimumWidth)
            if(L.minimumHeight > 0) natH = Math.max(natH, L.minimumHeight)
            if(L.maximumWidth  < Infinity) natW = Math.min(natW, L.maximumWidth)
            if(L.maximumHeight < Infinity) natH = Math.min(natH, L.maximumHeight)

            // alignSelf: read from FlexboxLayout group if present, else -1 = inherit
            const selfAlign = (child.FlexboxLayout && child.FlexboxLayout.alignSelf !== undefined)
                ? child.FlexboxLayout.alignSelf
                : FlexboxLayout.AlignAuto

            return {
                natW, natH,
                fillW: !!L.fillWidth,
                fillH: !!L.fillHeight,
                lm, rm, tm, bm,
                selfAlign,
                // outputs
                outW: natW, outH: natH, outX: 0, outY: 0,
            }
        })

        // ── natural main/cross sizes per item ──
        // mainSize = dimension along main axis (including margins)
        // crossSize = dimension along cross axis (including margins)
        const mainNat  = h => isRow ? (h.natW + h.lm + h.rm) : (h.natH + h.tm + h.bm)
        const crossNat = h => isRow ? (h.natH + h.tm + h.bm) : (h.natW + h.lm + h.rm)

        // ── step 1: wrap into flex lines ──
        const canWrap = wrapM !== FlexboxLayout.NoWrap
        const lines   = []   // each line: array of hint indices

        if(!canWrap || !(hasW && isRow) && !(hasH && !isRow)){
            // single line — all items
            lines.push(items.map((_, i) => i))
        } else {
            let line = []
            let mainUsed = 0
            for(let i = 0; i < hints.length; i++){
                const m = mainNat(hints[i])
                if(line.length > 0 && mainUsed + mainGap + m > containerMain){
                    lines.push(line)
                    line = [i]
                    mainUsed = m
                } else {
                    if(line.length > 0) mainUsed += mainGap
                    mainUsed += m
                    line.push(i)
                }
            }
            if(line.length) lines.push(line)
        }

        if(wrapM === FlexboxLayout.WrapReverse) lines.reverse()

        // ── step 2: resolve sizes for each line ──
        // For each line: distribute remaining main-axis space to fill items,
        // compute cross-axis size of line.
        const lineMainSizes  = []  // total main extent of each line (no gaps)
        const lineCrossSizes = []  // max cross size of items in each line

        for(const line of lines){
            let fixedMain = 0
            let fillCount = 0
            for(const i of line){
                const h = hints[i]
                const isFillMain = isRow ? h.fillW : h.fillH
                if(isFillMain) fillCount++
                else fixedMain += mainNat(h)
            }
            fixedMain += (line.length - 1) * mainGap

            // distribute extra main space to fill items
            if(fillCount > 0 && ((hasW && isRow) || (hasH && !isRow))){
                const extra = Math.max(0, containerMain - fixedMain)
                const each  = extra / fillCount
                for(const i of line){
                    const h = hints[i]
                    const isFillMain = isRow ? h.fillW : h.fillH
                    if(isFillMain){
                        if(isRow){ h.outW = Math.max(0, each - h.lm - h.rm) }
                        else      { h.outH = Math.max(0, each - h.tm - h.bm) }
                    }
                }
            }

            // recalculate line main total
            let totalMain = (line.length - 1) * mainGap
            for(const i of line) totalMain += isRow ? (hints[i].outW + hints[i].lm + hints[i].rm)
                                                     : (hints[i].outH + hints[i].tm + hints[i].bm)
            lineMainSizes.push(totalMain)

            // cross size of line
            let maxCross = 0
            for(const i of line) maxCross = Math.max(maxCross, crossNat(hints[i]))
            lineCrossSizes.push(maxCross)
        }

        // ── step 3: resolve container size ──
        const totalCrossGap = (lines.length - 1) * crossGap
        const totalCross = lineCrossSizes.reduce((s, v) => s + v, 0) + totalCrossGap
        const totalMain  = Math.max(...lineMainSizes)

        const finalW = hasW ? this.__proxy.width  : (isRow ? totalMain : totalCross)
        const finalH = hasH ? this.__proxy.height : (isRow ? totalCross : totalMain)
        const finalMain  = isRow ? finalW : finalH
        const finalCross = isRow ? finalH : finalW

        // ── step 4: distribute fill cross-axis items and alignContent ──
        // fill cross items: stretch to line cross size if alignItems = Stretch / fillH(isRow) / fillW(!isRow)
        {
            // alignContent: distribute extra cross space to lines
            const extraCross = Math.max(0, finalCross - totalCross)
            const lineOffsets = new Array(lines.length).fill(0)

            if(lines.length > 1 && extraCross > 0){
                switch(ac){
                    case FlexboxLayout.AlignCenter:
                        for(let l = 0; l < lines.length; l++) lineOffsets[l] = extraCross / 2
                        break
                    case FlexboxLayout.AlignEnd:
                        for(let l = 0; l < lines.length; l++) lineOffsets[l] = extraCross
                        break
                    case FlexboxLayout.AlignStretch: {
                        const add = extraCross / lines.length
                        for(let l = 0; l < lines.length; l++) lineCrossSizes[l] += add
                        break
                    }
                    case FlexboxLayout.AlignSpaceBetween: {
                        const sp = extraCross / (lines.length - 1)
                        for(let l = 1; l < lines.length; l++) lineOffsets[l] = sp * l
                        break
                    }
                    case FlexboxLayout.AlignSpaceAround: {
                        const sp = extraCross / lines.length
                        for(let l = 0; l < lines.length; l++) lineOffsets[l] = sp * 0.5 + sp * l
                        break
                    }
                    case FlexboxLayout.AlignSpaceEvenly: {
                        const sp = extraCross / (lines.length + 1)
                        for(let l = 0; l < lines.length; l++) lineOffsets[l] = sp * (l + 1)
                        break
                    }
                    // AlignStart: default — no offset
                }
            }

            // compute cross starts for each line
            let crossCursor = 0
            const lineCrossStart = []
            for(let l = 0; l < lines.length; l++){
                lineCrossStart.push(crossCursor + lineOffsets[l])
                crossCursor += lineCrossSizes[l] + crossGap
            }

            // ── step 5: compute item positions ──
            for(let l = 0; l < lines.length; l++){
                const line      = lines[l]
                const lineMain  = lineMainSizes[l]
                const lineCross = lineCrossSizes[l]
                const cs        = lineCrossStart[l]

                // justify-content: offset for main axis
                const extraMain = Math.max(0, finalMain - lineMain)
                let mainOffset = 0
                let mainSpaceBeween = 0
                let mainSpaceBefore = 0
                switch(jc){
                    case FlexboxLayout.JustifyCenter:      mainOffset = extraMain / 2; break
                    case FlexboxLayout.JustifyEnd:         mainOffset = extraMain; break
                    case FlexboxLayout.JustifySpaceBetween:
                        mainSpaceBeween = line.length > 1 ? extraMain / (line.length - 1) : 0; break
                    case FlexboxLayout.JustifySpaceAround:
                        mainSpaceBefore = extraMain / line.length / 2
                        mainSpaceBeween = extraMain / line.length; break
                    case FlexboxLayout.JustifySpaceEvenly:
                        mainSpaceBefore = extraMain / (line.length + 1)
                        mainSpaceBeween = mainSpaceBefore; break
                }

                let mainCursor = mainOffset + mainSpaceBefore
                const orderedLine = reversed ? [...line].reverse() : line

                for(const i of orderedLine){
                    const h = hints[i]

                    // cross align for this item
                    const effectiveAlign = (h.selfAlign !== FlexboxLayout.AlignAuto) ? h.selfAlign : ai
                    const itemCrossNat = crossNat(h)
                    let crossOffset = 0
                    let itemCrossSize = isRow ? h.outH : h.outW

                    if(effectiveAlign === FlexboxLayout.AlignStretch ||
                      (isRow ? h.fillH : h.fillW)){
                        // stretch to full line cross size (minus margins)
                        itemCrossSize = isRow
                            ? Math.max(0, lineCross - h.tm - h.bm)
                            : Math.max(0, lineCross - h.lm - h.rm)
                    } else {
                        switch(effectiveAlign){
                            case FlexboxLayout.AlignCenter:
                                crossOffset = (lineCross - itemCrossNat) / 2
                                break
                            case FlexboxLayout.AlignEnd:
                                crossOffset = lineCross - itemCrossNat
                                break
                            // AlignStart: default — crossOffset = 0
                        }
                        itemCrossSize = isRow ? h.natH : h.natW
                    }

                    // item main size
                    const itemMainSize = isRow ? h.outW : h.outH
                    const itemMainFull = itemMainSize + (isRow ? h.lm + h.rm : h.tm + h.bm)

                    // positions
                    const mainPos  = mainCursor + (isRow ? h.lm : h.tm)
                    const crossPos = cs + crossOffset + (isRow ? h.tm : h.lm)

                    if(isRow){
                        h.outX = mainPos
                        h.outY = crossPos
                        h.outW = itemMainSize
                        h.outH = itemCrossSize
                    } else {
                        h.outX = crossPos
                        h.outY = mainPos
                        h.outW = itemCrossSize
                        h.outH = itemMainSize
                    }

                    mainCursor += itemMainFull + mainGap + mainSpaceBeween
                }
            }
        }

        // ── PASS 2: set layout own size ──
        if(!hasW) Geometry.setAuto(this.__self, 'width',  finalW, this.__self.constructor.meta.width)
        if(!hasH) Geometry.setAuto(this.__self, 'height', finalH, this.__self.constructor.meta.height)

        // ── PASS 3: apply item sizes/positions ──
        for(let i = 0; i < items.length; i++){
            const h     = hints[i]
            const child = items[i]
            if(h.outX !== child.x) child.x = h.outX
            if(h.outY !== child.y) child.y = h.outY
            if(h.outW !== child.width)  child.width  = h.outW
            if(h.outH !== child.height) child.height = h.outH
        }

        this.__layouting = false
    }
}

module.exports = FlexboxLayout
