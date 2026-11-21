// ComboBox of QtQuick.Templates (qquickcombobox.cpp, Qt 6.12). Checked against Qt 6.12 (tests/diff/cases/combobox.qml):
//  - its model (an array, a number, a ListModel) and textRole / valueRole give count, currentText, currentValue (the row
//    itself without a valueRole) and displayText; currentIndex starts at 0 with rows;
//  - a click opens the popup (highlightedIndex the current one), a click on a delegate: highlighted, currentIndex,
//    activated, and the popup closes;
//  - closed, Up / Down change currentIndex (activated); Space opens the popup; open, Up / Down move highlightedIndex
//    (highlighted), Return / Enter take it (currentIndex, activated) and close, Escape closes.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { Control, withSetters, StrongFocus } = require('./Control')
const { ListModel } = require('../../QtQml/Models/ListModel')
const { modelRows } = require('../../QtQml/Models/ModelContext')
const { Icon } = require('./impl')
const QtEnums = require('../../Qt/enums')

class ComboBox extends Control {
    constructor(parent = null){
        super(parent)
        this.$plain(ComboBox.CURRENTINDEX, -1)
        this.$plain(ComboBox.HIGHLIGHTEDINDEX, -1)
        this.$plain(ComboBox.SELECTTEXTBYMOUSE, false)
        this.$plain(ComboBox.INPUTMETHODHINTS, 64)     // Qt.ImhNoPredictiveText
        this.$write(Control.FOCUSPOLICY, StrongFocus)
        this.$model = null
        this.$displaySet = false
        this.$downSet = false
        this.$pressed = false
    }

    // ---- the model ----
    $rows(){ return modelRows(this.$model) }
    $setModel(model){
        if(this.$model && this.$model.$removeView) this.$model.$removeView(this)
        this.$model = model
        if(model && model.$addView) model.$addView(this)
        this.$plain(ComboBox.MODEL, model)
        this.$modelReset()
    }
    $modelChanged(){ this.$modelReset() }
    // QQuickComboBoxPrivate::modelUpdated: the current row stays within the rows; a model with rows has one
    $modelReset(){
        let count = this.$rows().count
        this.$plain(ComboBox.COUNT, count)
        // QQuickComboBox::componentComplete: the current row of the model is taken once the combo box is complete
        if(!(this.$f & engine.COMPLETED)) return
        let current = this.$v[ComboBox.CURRENTINDEX]
        if(count === 0) current = -1
        else if(current >= count) current = count - 1
        else if(current < 0) current = 0
        this.$setCurrent(current, false)
    }
    // the text and the value of a row
    $roleOf(index, role){
        let rows = this.$rows()
        if(index < 0 || index >= rows.count) return undefined
        let row = rows.row(index), data = rows.data(index)
        if(role){
            if(row && typeof row === 'object' && role in row) return row[role]
            if(data && typeof data === 'object' && role in data) return data[role]
            return undefined
        }
        // no role: the value of a list (modelData); a ListModel has none
        return this.$model instanceof ListModel ? undefined : data
    }
    textAt(index){
        let value = this.$roleOf(index, this.$v[ComboBox.TEXTROLE])
        return value === undefined || value === null ? '' : String(value)
    }
    valueAt(index){
        let value = this.$roleOf(index, this.$v[ComboBox.VALUEROLE])
        return value === undefined && !this.$v[ComboBox.VALUEROLE] ? this.textAt(index) : value
    }
    indexOfValue(value){
        let count = this.$rows().count
        for(let i = 0; i < count; i++) if(this.valueAt(i) === value) return i
        return -1
    }
    // QQuickComboBox::find: Qt.MatchExactly by default
    find(text, flags = QtEnums.MatchExactly){
        let count = this.$rows().count
        let caseSensitive = !!(flags & QtEnums.MatchCaseSensitive)
        let norm = (s) => (caseSensitive ? s : s.toLowerCase())
        let want = norm(String(text))
        let kind = flags & 0xf
        for(let i = 0; i < count; i++){
            let t = norm(this.textAt(i))
            if(kind === QtEnums.MatchStartsWith ? t.startsWith(want) : kind === QtEnums.MatchEndsWith ? t.endsWith(want)
                : kind === QtEnums.MatchContains ? t.includes(want) : t === want) return i
        }
        return -1
    }
    $setCurrent(index, activated){
        let changed = this.$v[ComboBox.CURRENTINDEX] !== index
        this.$plain(ComboBox.CURRENTINDEX, index)
        this.$updateCurrent()
        if(activated) engine.emitArgs(this, ComboBox.SIGNAL_activated, [index])
        return changed
    }
    $updateCurrent(){
        let index = this.$v[ComboBox.CURRENTINDEX]
        let text = this.textAt(index)
        this.$plain(ComboBox.CURRENTTEXT, text)
        this.$plain(ComboBox.CURRENTVALUE, index >= 0 ? this.valueAt(index) : undefined)
        if(!this.$displaySet) this.$plain(ComboBox.DISPLAYTEXT, text)
        if(!this.$editSet) this.$plain(ComboBox.EDITTEXT, text)
    }
    incrementCurrentIndex(){ this.$step(1, false) }
    decrementCurrentIndex(){ this.$step(-1, false) }
    // QQuickComboBoxPrivate::increase / decrease: the current row (closed) or the highlighted one (open)
    $step(sign, user){
        let count = this.$v[ComboBox.COUNT]
        engine.batch(() => {
            if(this.$popupVisible()){
                let index = Math.max(0, Math.min(count - 1, this.$v[ComboBox.HIGHLIGHTEDINDEX] + sign))
                this.$setHighlighted(index, user)
            } else {
                let index = Math.max(0, Math.min(count - 1, this.$v[ComboBox.CURRENTINDEX] + sign))
                if(index !== this.$v[ComboBox.CURRENTINDEX]) this.$setCurrent(index, user)
            }
        })
    }
    $setHighlighted(index, user){
        if(this.$v[ComboBox.HIGHLIGHTEDINDEX] === index) return
        this.$plain(ComboBox.HIGHLIGHTEDINDEX, index)
        if(user && index >= 0) engine.emitArgs(this, ComboBox.SIGNAL_highlighted, [index])
    }
    selectAll(){
        let content = this.$v[Control.CONTENTITEM]
        if(content && content.selectAll) content.selectAll()
    }

    // ---- the popup ----
    $popup(){ return this.$v[ComboBox.POPUP] }
    $popupVisible(){ let p = this.$popup(); return !!(p && p.visible) }
    $setPopup(popup){
        let old = this.$v[ComboBox.POPUP]
        if(old === popup) return
        this.$plain(ComboBox.POPUP, popup || null)
        if(popup){
            if(!popup.$parent) popup.$setParent(this)
            engine.connect(popup, popup.constructor.SIGNAL_visibleChanged, () => this.$popupVisibleChanged(), null, engine.PRIORITY_NORMAL, this)
        }
    }
    $popupVisibleChanged(){
        let visible = this.$popupVisible()
        // QQuickComboBoxPrivate::popupVisibleChanged: open, the current row is highlighted; closed, none
        this.$setHighlighted(visible ? this.$v[ComboBox.CURRENTINDEX] : -1, false)
        if(!this.$downSet) this.$plain(ComboBox.DOWN, this.$pressed || visible)
    }
    $showPopup(){ let p = this.$popup(); if(p && !p.visible) p.open() }
    // QQuickComboBoxPrivate::hidePopup(accept): accepting takes the highlighted row
    $hidePopup(accept){
        let p = this.$popup()
        if(!p || !p.visible) return
        if(accept){
            let index = this.$v[ComboBox.HIGHLIGHTEDINDEX]
            if(index >= 0) this.$setCurrent(index, true)
        }
        p.close()
    }
    $togglePopup(accept){
        if(this.$popupVisible()) this.$hidePopup(accept)
        else this.$showPopup()
    }
    // a delegate of the popup: its click takes its row, its hover highlights it (QQuickComboBoxPrivate::createdItem)
    $connectDelegate(item, indexOf){
        if(item.$comboConnected) return
        item.$comboConnected = true
        let clicked = item.constructor.SIGNAL_clicked
        if(clicked !== undefined) engine.connect(item, clicked, () => {
            let index = indexOf()
            this.$setHighlighted(index, true)
            this.$hidePopup(true)
        }, null, engine.PRIORITY_NORMAL, this)
        let hovered = item.constructor.SIGNAL_hoveredChanged
        if(hovered !== undefined) engine.connect(item, hovered, () => {
            if(item.hovered && this.$v[ComboBox.HIGHLIGHTONHOVER]) this.$setHighlighted(indexOf(), true)
        }, null, engine.PRIORITY_NORMAL, this)
    }

    // ---- press, keys ----
    $setPressed(value){
        this.$pressed = value
        this.$plain(ComboBox.PRESSED, value)
        if(!this.$downSet) this.$plain(ComboBox.DOWN, value || this.$popupVisible())
    }
    $mousePress(x, y, button){
        if(button !== 1) return false
        this.$clickFocus()
        this.$setPressed(true)
        return true
    }
    $mouseMove(x, y){ if(this.$pressed) this.$setPressed(x >= 0 && y >= 0 && x <= this.$v[Item.WIDTH] && y <= this.$v[Item.HEIGHT]) }
    $mouseRelease(x, y){
        let was = this.$pressed
        this.$setPressed(false)
        if(was) this.$togglePopup(false)
    }
    $ungrabMouse(){ this.$setPressed(false) }
    $keyEvent(event, kind){
        let K = QtEnums
        let open = this.$popupVisible()
        if(kind !== 'pressed'){
            if(event.key === K.Key_Space && this.$spacePressed){
                this.$spacePressed = false
                this.$setPressed(false)
                this.$togglePopup(false)
                return 1
            }
            return 0
        }
        switch(event.key){
        case K.Key_Space:
            if(event.isAutoRepeat) return 1
            this.$spacePressed = true
            this.$setPressed(true)
            return 1
        case K.Key_Up: this.$step(-1, true); return 1
        case K.Key_Down: this.$step(1, true); return 1
        case K.Key_Home:
            if(open) this.$setHighlighted(0, true)
            else if(this.$v[ComboBox.COUNT]) this.$setCurrent(0, true)
            return 1
        case K.Key_End:
            if(open) this.$setHighlighted(this.$v[ComboBox.COUNT] - 1, true)
            else if(this.$v[ComboBox.COUNT]) this.$setCurrent(this.$v[ComboBox.COUNT] - 1, true)
            return 1
        case K.Key_Return:
        case K.Key_Enter:
            if(open){
                this.$hidePopup(true)
                return 1
            }
            if(this.$v[ComboBox.EDITABLE]) engine.emit(this, ComboBox.SIGNAL_accepted)
            return 0
        case K.Key_Escape:
            if(open){
                this.$hidePopup(false)
                return 1
            }
            return 0
        }
        return 0
    }
    $wheel(x, y, angleDelta){
        if(!this.$v[Control.WHEELENABLED]) return false
        let delta = angleDelta.y || angleDelta.x
        if(delta) this.$step(delta > 0 ? -1 : 1, true)
        return true
    }
    $delegateChanged(id){
        if(id === ComboBox.INDICATOR){
            let indicator = this.$v[ComboBox.INDICATOR]
            this.$plain(ComboBox.IMPLICITINDICATORWIDTH, indicator ? indicator.$v[Item.IMPLICITWIDTH] : 0)
            this.$plain(ComboBox.IMPLICITINDICATORHEIGHT, indicator ? indicator.$v[Item.IMPLICITHEIGHT] : 0)
            return
        }
        super.$delegateChanged(id)
    }
    $delegateImplicitChanged(id){
        if(id === ComboBox.INDICATOR) return this.$delegateChanged(id)
        super.$delegateImplicitChanged(id)
    }
    $componentComplete(){
        super.$componentComplete()
        this.$modelReset()
    }
    $disposeNative(){
        if(this.$model && this.$model.$removeView) this.$model.$removeView(this)
        super.$disposeNative()
    }
}
defineType(ComboBox, {
    props: {
        count: { kind: 'int', readonly: true }, model: 'var', delegateModel: { kind: 'var', readonly: true, initial: null },
        pressed: { kind: 'bool', readonly: true }, highlightedIndex: { kind: 'int', readonly: true }, currentIndex: 'int',
        currentText: { kind: 'string', readonly: true }, displayText: 'string', textRole: 'string', valueRole: 'string',
        currentValue: { kind: 'var', readonly: true }, delegate: { kind: 'var', initial: null }, indicator: { kind: 'var', initial: null },
        popup: { kind: 'var', initial: null }, flat: 'bool', down: 'bool', editable: 'bool', editText: 'string',
        validator: { kind: 'var', initial: null }, inputMethodHints: 'int', inputMethodComposing: { kind: 'bool', readonly: true },
        acceptableInput: { kind: 'bool', readonly: true, initial: true },
        implicitIndicatorWidth: { kind: 'real', readonly: true }, implicitIndicatorHeight: { kind: 'real', readonly: true },
        selectTextByMouse: 'bool', implicitContentWidthPolicy: 'int', highlightOnHover: { kind: 'bool', initial: true },
    },
    signals: ['activated(index)', 'highlighted(index)', 'accepted'],
})
ComboBox.$setters = withSetters(Control, [
    [ComboBox.MODEL, (c, v) => c.$setModel(v)],
    [ComboBox.CURRENTINDEX, (c, v) => c.$setCurrent(v === undefined ? -1 : v, false)],
    [ComboBox.TEXTROLE, (c, v) => { c.$plain(ComboBox.TEXTROLE, v || ''); c.$updateCurrent() }],
    [ComboBox.VALUEROLE, (c, v) => { c.$plain(ComboBox.VALUEROLE, v || ''); c.$updateCurrent() }],
    [ComboBox.DISPLAYTEXT, (c, v) => { c.$displaySet = v !== undefined; c.$plain(ComboBox.DISPLAYTEXT, v === undefined ? c.$v[ComboBox.CURRENTTEXT] : String(v)) }],
    [ComboBox.EDITTEXT, (c, v) => { c.$editSet = v !== undefined; c.$plain(ComboBox.EDITTEXT, v === undefined ? c.$v[ComboBox.CURRENTTEXT] : String(v)) }],
    [ComboBox.DOWN, (c, v) => { c.$downSet = v !== undefined; c.$plain(ComboBox.DOWN, v === undefined ? c.$pressed || c.$popupVisible() : !!v) }],
    [ComboBox.POPUP, (c, v) => c.$setPopup(v)],
    [ComboBox.INDICATOR, (c, v) => c.$setDelegate(ComboBox.INDICATOR, v)],
])
Object.assign(ComboBox, { ContentItemWidth: 0, WidestText: 1, WidestTextWhenCompleted: 2 })

module.exports = { ComboBox }
