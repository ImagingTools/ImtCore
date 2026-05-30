const Item = require("../Item")
const Repeater = require("../Repeater")
const Geometry = require("../../QtQml/Geometry")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Bool = require("../../QtQml/Bool")
const Signal = require("../../QtQml/Signal")
const QtEnums = require("../../Qt/enums")

class GridLayout extends Item {
    static LeftToRight = 0
    static TopToBottom = 1

    static meta = Object.assign({}, Item.meta, {
        columns:             { type: Int,  value: -1 },   // -1 = unlimited
        rows:                { type: Int,  value: -1 },   // -1 = unlimited
        flow:                { type: Int,  value: GridLayout.LeftToRight },
        layoutDirection:     { type: Int,  value: 0 },    // 0 = LeftToRight
        columnSpacing:       { type: Real, value: 5 },
        rowSpacing:          { type: Real, value: 5 },
        uniformCellWidths:   { type: Bool, value: false },
        uniformCellHeights:  { type: Bool, value: false },

        columnsChanged:           { type: Signal, args: [] },
        rowsChanged:              { type: Signal, args: [] },
        flowChanged:              { type: Signal, args: [] },
        layoutDirectionChanged:   { type: Signal, args: [] },
        columnSpacingChanged:     { type: Signal, args: [] },
        rowSpacingChanged:        { type: Signal, args: [] },
        uniformCellWidthsChanged: { type: Signal, args: [] },
        uniformCellHeightsChanged:{ type: Signal, args: [] },
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__DOM.classList.add('GridLayout')
        return obj
    }

    __updateProperties(){
        super.__updateProperties()
        this.__updateGeometry(true)
    }

    'SLOT_Component.completed'(){
        JQApplication.updateLater(this)
    }

    SLOT_columnSpacingChanged(){ JQApplication.updateLater(this) }
    SLOT_rowSpacingChanged()   { JQApplication.updateLater(this) }
    SLOT_columnsChanged()      { JQApplication.updateLater(this) }
    SLOT_rowsChanged()         { JQApplication.updateLater(this) }
    SLOT_flowChanged()         { JQApplication.updateLater(this) }
    SLOT_uniformCellWidthsChanged()  { JQApplication.updateLater(this) }
    SLOT_uniformCellHeightsChanged() { JQApplication.updateLater(this) }

    SLOT_visibleChanged(oldValue, newValue){
        JQApplication.beginUpdate()
        super.SLOT_visibleChanged(oldValue, newValue)
        JQApplication.endUpdate()
    }

    __updateGeometry(completed = false){
        if(!completed || !this.__proxy.visible) return
        if(this.__layouting) return
        this.__layouting = true

        const colSpacing = this.__proxy.columnSpacing
        const rowSpacing = this.__proxy.rowSpacing
        const flow       = this.__proxy.flow
        const maxCols    = this.__proxy.columns  // -1 = unlimited
        const maxRows    = this.__proxy.rows     // -1 = unlimited
        const hasW       = !!this.__self.width__prevent
        const hasH       = !!this.__self.height__prevent
        const layoutW    = this.__proxy.width
        const layoutH    = this.__proxy.height

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
            // explicit cell position (-1 = auto)
            const explicitRow = L.row    >= 0 ? L.row    : -1
            const explicitCol = L.column >= 0 ? L.column : -1
            return {
                fillW:   !!L.fillWidth,
                fillH:   !!L.fillHeight,
                minW:    L.minimumWidth  || 0,
                maxW:    L.maximumWidth  < Infinity ? L.maximumWidth  : Infinity,
                minH:    L.minimumHeight || 0,
                maxH:    L.maximumHeight < Infinity ? L.maximumHeight : Infinity,
                prefW:   L.preferredWidth,
                prefH:   L.preferredHeight,
                hStr:    L.horizontalStretchFactor > 0 ? L.horizontalStretchFactor : 1,
                vStr:    L.verticalStretchFactor   > 0 ? L.verticalStretchFactor   : 1,
                align:   L.alignment,
                colSpan: L.columnSpan > 0 ? L.columnSpan : 1,
                rowSpan: L.rowSpan    > 0 ? L.rowSpan    : 1,
                explicitRow, explicitCol,
                lm, rm, tm, bm,
                naturalW: 0, naturalH: 0,
                itemW: 0, itemH: 0,
                gridRow: 0, gridCol: 0,
                outX: 0, outY: 0,
            }
        })

        // ── compute natural sizes ─────────────────────────────────────────
        for(let i = 0; i < items.length; i++){
            const h = hints[i]
            let w = h.prefW > 0 ? h.prefW : (items[i].width  || 0)
            if(h.minW > 0)        w = Math.max(w, h.minW)
            if(h.maxW < Infinity) w = Math.min(w, h.maxW)
            h.naturalW = Math.max(0, w)

            let hh = h.prefH > 0 ? h.prefH : (items[i].height || 0)
            if(h.minH > 0)        hh = Math.max(hh, h.minH)
            if(h.maxH < Infinity) hh = Math.min(hh, h.maxH)
            h.naturalH = Math.max(0, hh)
        }

        // ── auto-place items into grid cells ──────────────────────────────
        // occupied[row][col] = true
        const occupied = {}
        function isOccupied(r, c, rSpan, cSpan){
            for(let dr = 0; dr < rSpan; dr++)
                for(let dc = 0; dc < cSpan; dc++)
                    if(occupied[r+dr] && occupied[r+dr][c+dc]) return true
            return false
        }
        function occupy(r, c, rSpan, cSpan){
            for(let dr = 0; dr < rSpan; dr++){
                if(!occupied[r+dr]) occupied[r+dr] = {}
                for(let dc = 0; dc < cSpan; dc++)
                    occupied[r+dr][c+dc] = true
            }
        }

        // first: place items with explicit row/col
        for(let i = 0; i < items.length; i++){
            const h = hints[i]
            if(h.explicitRow >= 0 && h.explicitCol >= 0){
                h.gridRow = h.explicitRow
                h.gridCol = h.explicitCol
                occupy(h.gridRow, h.gridCol, h.rowSpan, h.colSpan)
            }
        }

        // second: auto-place remaining items
        let autoR = 0, autoC = 0
        for(let i = 0; i < items.length; i++){
            const h = hints[i]
            if(h.explicitRow >= 0 && h.explicitCol >= 0) continue

            if(flow === GridLayout.LeftToRight){
                const colLimit = maxCols > 0 ? maxCols : Infinity
                while(true){
                    if(autoC + h.colSpan > colLimit){ autoC = 0; autoR++ }
                    if(!isOccupied(autoR, autoC, h.rowSpan, h.colSpan)) break
                    autoC++
                    if(autoC + h.colSpan > colLimit){ autoC = 0; autoR++ }
                }
                h.gridRow = autoR; h.gridCol = autoC
                occupy(h.gridRow, h.gridCol, h.rowSpan, h.colSpan)
                autoC += h.colSpan
            } else {
                // TopToBottom
                const rowLimit = maxRows > 0 ? maxRows : Infinity
                while(true){
                    if(autoR + h.rowSpan > rowLimit){ autoR = 0; autoC++ }
                    if(!isOccupied(autoR, autoC, h.rowSpan, h.colSpan)) break
                    autoR++
                    if(autoR + h.rowSpan > rowLimit){ autoR = 0; autoC++ }
                }
                h.gridRow = autoR; h.gridCol = autoC
                occupy(h.gridRow, h.gridCol, h.rowSpan, h.colSpan)
                autoR += h.rowSpan
            }
        }

        // ── determine grid dimensions ─────────────────────────────────────
        let numCols = 0, numRows = 0
        for(const h of hints){
            numCols = Math.max(numCols, h.gridCol + h.colSpan)
            numRows = Math.max(numRows, h.gridRow + h.rowSpan)
        }

        // ── compute natural column widths & row heights ───────────────────
        // Only consider non-spanning items for base track sizes
        const colW = new Array(numCols).fill(0)
        const rowH = new Array(numRows).fill(0)

        for(const h of hints){
            if(h.colSpan === 1) colW[h.gridCol] = Math.max(colW[h.gridCol], h.naturalW + h.lm + h.rm)
            if(h.rowSpan === 1) rowH[h.gridRow] = Math.max(rowH[h.gridRow], h.naturalH + h.tm + h.bm)
        }

        // uniformCellWidths / uniformCellHeights
        if(this.__proxy.uniformCellWidths){
            const maxW = Math.max(...colW)
            colW.fill(maxW)
        }
        if(this.__proxy.uniformCellHeights){
            const maxH = Math.max(...rowH)
            rowH.fill(maxH)
        }

        // ── distribute extra space from explicit layout size ──────────────
        // Columns: gather fill columns
        const fillCols = []
        for(let c = 0; c < numCols; c++){
            // a column is "fill" if any item with fillWidth spans only it
            for(const h of hints){
                if(h.colSpan === 1 && h.gridCol === c && h.fillW){ fillCols.push(c); break }
            }
        }
        const fillRows = []
        for(let r = 0; r < numRows; r++){
            for(const h of hints){
                if(h.rowSpan === 1 && h.gridRow === r && h.fillH){ fillRows.push(r); break }
            }
        }

        if(hasW && fillCols.length === 0){
            // proportional cell distribution like RowLayout
            const fixedSum = colW.reduce((s, w) => s + w, 0) + (numCols - 1) * colSpacing
            const available = layoutW - (numCols - 1) * colSpacing
            const totalNat  = colW.reduce((s, w) => s + w, 0)
            if(totalNat > 0){
                for(let c = 0; c < numCols; c++) colW[c] = available * colW[c] / totalNat
            }
        } else if(hasW && fillCols.length > 0){
            const fixedSum = colW.reduce((s, w, i) => fillCols.includes(i) ? s : s + w, 0)
                           + (numCols - 1) * colSpacing
            const extra = layoutW - fixedSum
            const totalStr = fillCols.reduce((s, c) => {
                let str = 0
                for(const h of hints) if(h.colSpan === 1 && h.gridCol === c && h.fillW) str = Math.max(str, h.hStr)
                return s + str
            }, 0)
            for(const c of fillCols){
                let str = 0
                for(const h of hints) if(h.colSpan === 1 && h.gridCol === c && h.fillW) str = Math.max(str, h.hStr)
                colW[c] = Math.max(0, extra * str / totalStr)
            }
        }

        if(hasH && fillRows.length === 0){
            const available = layoutH - (numRows - 1) * rowSpacing
            const totalNat  = rowH.reduce((s, h) => s + h, 0)
            if(totalNat > 0){
                for(let r = 0; r < numRows; r++) rowH[r] = available * rowH[r] / totalNat
            }
        } else if(hasH && fillRows.length > 0){
            const fixedSum = rowH.reduce((s, h, i) => fillRows.includes(i) ? s : s + h, 0)
                           + (numRows - 1) * rowSpacing
            const extra = layoutH - fixedSum
            const totalStr = fillRows.reduce((s, r) => {
                let str = 0
                for(const h of hints) if(h.rowSpan === 1 && h.gridRow === r && h.fillH) str = Math.max(str, h.vStr)
                return s + str
            }, 0)
            for(const r of fillRows){
                let str = 0
                for(const h of hints) if(h.rowSpan === 1 && h.gridRow === r && h.fillH) str = Math.max(str, h.vStr)
                rowH[r] = Math.max(0, extra * str / totalStr)
            }
        }

        // ── compute column X positions and row Y positions ────────────────
        const colX = new Array(numCols)
        const rowY = new Array(numRows)
        let cx = 0
        for(let c = 0; c < numCols; c++){ colX[c] = cx; cx += colW[c] + colSpacing }
        let ry = 0
        for(let r = 0; r < numRows; r++){ rowY[r] = ry; ry += rowH[r] + rowSpacing }

        const finalW = hasW ? layoutW : cx - colSpacing
        const finalH = hasH ? layoutH : ry - rowSpacing

        // ── compute item sizes and positions ──────────────────────────────
        const fillHints = []
        const nonFillHints = []

        for(let i = 0; i < items.length; i++){
            const h = hints[i]

            // cell area for this item (may span multiple cols/rows)
            const cellW = colW.slice(h.gridCol, h.gridCol + h.colSpan).reduce((s, w) => s + w, 0)
                        + (h.colSpan - 1) * colSpacing
            const cellH = rowH.slice(h.gridRow, h.gridRow + h.rowSpan).reduce((s, hh) => s + hh, 0)
                        + (h.rowSpan - 1) * rowSpacing

            // item size
            if(h.fillW){
                h.itemW = Math.max(0, cellW - h.lm - h.rm)
                if(h.minW > 0)        h.itemW = Math.max(h.itemW, h.minW)
                if(h.maxW < Infinity) h.itemW = Math.min(h.itemW, h.maxW)
            } else {
                h.itemW = h.naturalW
            }

            if(h.fillH){
                h.itemH = Math.max(0, cellH - h.tm - h.bm)
                if(h.minH > 0)        h.itemH = Math.max(h.itemH, h.minH)
                if(h.maxH < Infinity) h.itemH = Math.min(h.itemH, h.maxH)
            } else {
                h.itemH = h.naturalH
            }

            // horizontal position in cell
            const ax = colX[h.gridCol]
            if     (h.fillW)                            h.outX = ax + h.lm
            else if(h.align & QtEnums.AlignHCenter)     h.outX = ax + Math.round((cellW - h.itemW) / 2)
            else if(h.align & QtEnums.AlignRight)       h.outX = ax + cellW - h.itemW - h.rm
            else                                        h.outX = ax + h.lm  // AlignLeft

            // vertical position in cell
            const ay = rowY[h.gridRow]
            if     (h.fillH)                            h.outY = ay + h.tm
            else if(h.align & QtEnums.AlignTop)         h.outY = ay + h.tm
            else if(h.align & QtEnums.AlignBottom)      h.outY = ay + cellH - h.itemH - h.bm
            else                                        h.outY = ay + Math.round((cellH - h.itemH) / 2)  // AlignVCenter default

            if(h.fillW || h.fillH) fillHints.push(i)
            else                   nonFillHints.push(i)
        }

        // ── PASS 2: set layout own size FIRST ─────────────────────────────
        if(!hasW) Geometry.setAuto(this.__self, 'width',  finalW, this.__self.constructor.meta.width)
        if(!hasH) Geometry.setAuto(this.__self, 'height', finalH, this.__self.constructor.meta.height)

        // ── PASS 3: fill items first, then non-fill ────────────────────────
        const applyOrder = [...fillHints, ...nonFillHints]
        for(const i of applyOrder){
            const h     = hints[i]
            const child = items[i]

            if(h.outX !== child.x) child.x = h.outX
            if(h.outY !== child.y) child.y = h.outY
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

module.exports = GridLayout
