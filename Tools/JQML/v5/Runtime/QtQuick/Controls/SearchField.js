// SearchField of QtQuick.Templates (qquicksearchfield.cpp, Qt 6.10+). Checked against Qt 6.12
// (tests/diff/cases/searchfield.qml):
//  - text: the text of its content item (a TextField); an edit by the user: textEdited, no current suggestion,
//    searchTriggered when live; a text with suggestions opens the popup, an empty one or none closes it;
//  - the popup open: the current suggestion highlighted, else the first; Up / Down / Home / End move the highlight;
//    Return / Enter take the highlighted one (its text, activated) and emit accepted and searchTriggered; Escape closes the
//    popup, else clears the text; a click on a suggestion takes it;
//  - searchIndicator: a press focuses the text and emits searchButtonPressed; clearIndicator: a press clears the text
//    (clearButtonPressed); a suggestion of the same text as the text becomes current when the suggestions change.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { Control, withSetters } = require('./Control')
const { IndicatorButton } = require('./Range')
const { modelRows } = require('../../QtQml/Models/ModelContext')
const { ListModel } = require('../../QtQml/Models/ListModel')
const QtEnums = require('../../Qt/enums')

const StrongFocus = 11

class SearchField extends Control {
    constructor(parent = null){
        super(parent)
        this.$plain(SearchField.CURRENTINDEX, -1)
        this.$plain(SearchField.HIGHLIGHTEDINDEX, -1)
        this.$plain(SearchField.LIVE, true)
        this.$plain(SearchField.SELECTTEXTBYMOUSE, true)
        this.$v[SearchField.SEARCHINDICATOR] = new IndicatorButton(this)
        this.$v[SearchField.CLEARINDICATOR] = new IndicatorButton(this)
        this.$write(Control.FOCUSPOLICY, StrongFocus)
        this.$model = null
    }
    // ---- the suggestions ----
    $rows(){ return modelRows(this.$model) }
    $setModel(model){
        if(this.$model && this.$model.$removeView) this.$model.$removeView(this)
        this.$model = model
        if(model && model.$addView) model.$addView(this)
        this.$plain(SearchField.SUGGESTIONMODEL, model)
        this.$plain(SearchField.DELEGATEMODEL, model === undefined || model === null ? null : model)
        this.$countChanged()
    }
    $modelChanged(){ this.$countChanged() }
    // QQuickSearchFieldPrivate::suggestionCountChanged
    $countChanged(){
        let count = this.$rows().count
        this.$plain(SearchField.SUGGESTIONCOUNT, count)
        if(count === 0){
            this.$plain(SearchField.CURRENTINDEX, -1)
            this.$setHighlighted(-1, false)
            return
        }
        let text = this.$v[SearchField.TEXT]
        if(text){
            for(let i = 0; i < count; i++){
                if(this.$textAt(i) === text){
                    this.$setCurrentItem(i, false)
                    this.$updateHighlighted()
                    break
                }
            }
        }
    }
    // the text of a suggestion: its textRole (modelData without one)
    $textAt(index){
        let rows = this.$rows()
        if(index < 0 || index >= rows.count) return ''
        let role = this.$v[SearchField.TEXTROLE]
        let row = rows.row(index), data = rows.data(index)
        let value
        if(role){
            if(row && typeof row === 'object' && role in row) value = row[role]
            else if(data && typeof data === 'object' && role in data) value = data[role]
        } else value = this.$model instanceof ListModel ? undefined : data
        return value === undefined || value === null ? '' : String(value)
    }
    textAt(index){ return this.$textAt(index) }
    $setCurrentItem(index, activate){
        if(this.$v[SearchField.CURRENTINDEX] === index) return
        this.$plain(SearchField.CURRENTINDEX, index)
        // updateDisplayText
        if(index >= 0){
            let text = this.$textAt(index)
            if(text !== this.$v[SearchField.TEXT]) this.text = text
        }
        if(activate) engine.emitArgs(this, SearchField.SIGNAL_activated, [index])
    }
    $updateHighlighted(){
        let index = -1
        if(this.$popupVisible()){
            if(this.$v[SearchField.CURRENTINDEX] >= 0) index = this.$v[SearchField.CURRENTINDEX]
            else if(this.$v[SearchField.SUGGESTIONCOUNT] > 0) index = 0
        }
        this.$setHighlighted(index, false)
    }
    $setHighlighted(index, user){
        if(this.$v[SearchField.HIGHLIGHTEDINDEX] === index) return
        this.$plain(SearchField.HIGHLIGHTEDINDEX, index)
        if(user) engine.emitArgs(this, SearchField.SIGNAL_highlighted, [index])
    }

    // ---- the popup ----
    $popup(){ return this.$v[SearchField.POPUP] }
    $popupVisible(){ let p = this.$popup(); return !!(p && p.visible) }
    $setPopup(popup){
        let old = this.$v[SearchField.POPUP]
        if(old === popup) return
        if(old){
            old.visible = false
            engine.disconnect(old, old.constructor.SIGNAL_visibleChanged, null, null, this)
        }
        this.$plain(SearchField.POPUP, popup || null)
        if(popup){
            if(!popup.$parent) popup.$setParent(this)
            // CloseOnEscape | CloseOnPressOutsideParent
            if(popup.closePolicy !== undefined) popup.closePolicy = 0x1 | 0x8
            engine.connect(popup, popup.constructor.SIGNAL_visibleChanged, () => this.$updateHighlighted(), null, engine.PRIORITY_NORMAL, this)
        }
    }
    $showPopup(){ let p = this.$popup(); if(p && !p.visible) p.open() }
    // QQuickSearchFieldPrivate::hidePopup(accept): accepting takes the highlighted suggestion
    $hidePopup(accept){
        if(accept){
            let index = this.$v[SearchField.HIGHLIGHTEDINDEX]
            this.$setCurrentItem(index, false)
            engine.emitArgs(this, SearchField.SIGNAL_activated, [index])
        }
        let p = this.$popup()
        if(p && p.visible) p.close()
    }
    // a delegate of the popup: a click takes its suggestion, a hover highlights it (QQuickSearchFieldPrivate::createdItem)
    $connectDelegate(item, indexOf){
        if(item.$searchConnected) return
        item.$searchConnected = true
        if(item.focusPolicy !== undefined) item.focusPolicy = 0
        let clicked = item.constructor.SIGNAL_clicked
        if(clicked !== undefined) engine.connect(item, clicked, () => {
            let index = indexOf()
            if(index < 0) return
            this.$setHighlighted(index, true)
            this.$hidePopup(true)
        }, null, engine.PRIORITY_NORMAL, this)
        let hovered = item.constructor.SIGNAL_hoveredChanged
        if(hovered !== undefined) engine.connect(item, hovered, () => {
            if(item.hovered && item.enabled) this.$setHighlighted(indexOf(), true)
        }, null, engine.PRIORITY_NORMAL, this)
    }

    // ---- the text of the content (QQuickSearchFieldPrivate::updateText) ----
    $field(){ let c = this.$v[Control.CONTENTITEM]; return c && c.$v && 'selectByMouse' in c ? c : null }
    $inputTextChanged(){
        let input = this.$field()
        if(!input) return
        let text = input.text
        if(this.$v[SearchField.TEXT] !== text){
            this.text = text
            engine.emit(this, SearchField.SIGNAL_textEdited)
            this.$plain(SearchField.CURRENTINDEX, -1)
            this.$updateHighlighted()
            if(this.$v[SearchField.LIVE]) engine.emit(this, SearchField.SIGNAL_searchTriggered)
        }
        if(!this.$v[SearchField.TEXT]){
            if(this.$popupVisible()) this.$hidePopup(false)
        } else if(this.$v[SearchField.SUGGESTIONCOUNT] > 0){
            if(!this.$popupVisible()) this.$showPopup()
        } else if(this.$popupVisible()) this.$hidePopup(false)
    }
    $delegateChanged(id){
        super.$delegateChanged(id)
        if(id !== Control.CONTENTITEM) return
        let input = this.$field()
        if(!input || input.$searchWatched) return
        input.$searchWatched = true
        engine.connect(input, input.constructor.SIGNAL_textChanged, () => this.$inputTextChanged(), null, engine.PRIORITY_NORMAL, this)
        let sync = () => {
            this.$plain(SearchField.CURSORPOSITION, input.cursorPosition)
            this.$plain(SearchField.SELECTEDTEXT, input.selectedText)
            this.$plain(SearchField.SELECTIONSTART, input.selectionStart)
            this.$plain(SearchField.SELECTIONEND, input.selectionEnd)
        }
        for(let name of ['cursorPositionChanged', 'selectedTextChanged', 'selectionStartChanged', 'selectionEndChanged']){
            let signal = input.constructor['SIGNAL_' + name]
            if(signal !== undefined) engine.connect(input, signal, sync, null, engine.PRIORITY_NORMAL, this)
        }
        // a release on the text with suggestions opens the popup (QQuickSearchField::eventFilter)
        let release = input.$mouseRelease
        input.$mouseRelease = function(x, y, button){
            let r = release ? release.apply(this, arguments) : undefined
            let field = this.$searchField
            if(field && this.activeFocus && field.$v[SearchField.TEXT] && !field.$popupVisible() && field.$v[SearchField.SUGGESTIONCOUNT] > 0 && (button === undefined || button === 1)) field.$showPopup()
            return r
        }
        input.$searchField = this
        // the focus leaves the text: the popup closes (QQuickSearchField::eventFilter FocusOut)
        engine.connect(input, Item.SIGNAL_activeFocusChanged, () => {
            if(!input.activeFocus && !this.$v[Item.ACTIVEFOCUS]){
                let p = this.$popup()
                if(!(p && p.activeFocus)) this.$hidePopup(false)
            }
        }, null, engine.PRIORITY_NORMAL, this)
    }
    select(start, end){ let i = this.$field(); if(i) i.select(start, end) }
    selectWord(){ let i = this.$field(); if(i) i.selectWord() }
    selectAll(){ let i = this.$field(); if(i) i.selectAll() }
    deselect(){ let i = this.$field(); if(i) i.deselect() }

    // ---- press on the indicators (QQuickSearchFieldPrivate::handlePress / handleRelease) ----
    $indicatorAt(id, x, y){
        let indicator = this.$v[id].$v[IndicatorButton.INDICATOR]
        if(!indicator || !indicator.enabled || !indicator.$v[Item.VISIBLE]) return false
        let p = this.mapToItem(indicator, x, y)
        return p.x >= 0 && p.y >= 0 && p.x <= indicator.$v[Item.WIDTH] && p.y <= indicator.$v[Item.HEIGHT]
    }
    $mousePress(x, y, button){
        if(button !== 1) return false
        if(this.$indicatorAt(SearchField.SEARCHINDICATOR, x, y)){
            this.$v[SearchField.SEARCHINDICATOR].$write(IndicatorButton.PRESSED, true)
            let input = this.$field()
            if(input){
                input.forceActiveFocus()
                engine.emit(this, SearchField.SIGNAL_searchButtonPressed)
            }
        } else if(this.$indicatorAt(SearchField.CLEARINDICATOR, x, y)){
            this.$v[SearchField.CLEARINDICATOR].$write(IndicatorButton.PRESSED, true)
            if(this.$v[SearchField.TEXT]){
                this.$plain(SearchField.CURRENTINDEX, -1)
                this.$updateHighlighted()
                this.text = ''
                if(this.$popupVisible()) this.$hidePopup(false)
                engine.emit(this, SearchField.SIGNAL_clearButtonPressed)
            }
        }
        return true
    }
    $mouseRelease(){
        for(let id of [SearchField.SEARCHINDICATOR, SearchField.CLEARINDICATOR]){
            let button = this.$v[id]
            if(button.$v[IndicatorButton.PRESSED]){ button.$write(IndicatorButton.PRESSED, false); break }
        }
    }
    $ungrabMouse(){ this.$mouseRelease() }
    // the hover of the indicators
    $hoverEnter(x, y){ super.$hoverEnter(x, y); this.$hoverIndicators(x, y) }
    $hoverMove(x, y){ super.$hoverMove(x, y); this.$hoverIndicators(x, y) }
    $hoverIndicators(x, y){
        GroupWrite(this.$v[SearchField.SEARCHINDICATOR], IndicatorButton.HOVERED, this.$indicatorAt(SearchField.SEARCHINDICATOR, x, y))
        GroupWrite(this.$v[SearchField.CLEARINDICATOR], IndicatorButton.HOVERED, this.$indicatorAt(SearchField.CLEARINDICATOR, x, y))
    }
    $hoverLeave(){
        super.$hoverLeave()
        GroupWrite(this.$v[SearchField.SEARCHINDICATOR], IndicatorButton.HOVERED, false)
        GroupWrite(this.$v[SearchField.CLEARINDICATOR], IndicatorButton.HOVERED, false)
    }

    // ---- keys (QQuickSearchField::keyPressEvent): the ones the text does not take ----
    $keyEvent(event, kind){
        if(kind !== 'pressed') return 0
        let key = event.key
        let hasModel = this.$model !== null && this.$model !== undefined
        let count = this.$v[SearchField.SUGGESTIONCOUNT]
        let highlighted = this.$v[SearchField.HIGHLIGHTEDINDEX]
        switch(key){
        case QtEnums.Key_Escape: case QtEnums.Key_Back:
            if(this.$popupVisible()){ this.$hidePopup(false); return 1 }
            if(this.$v[SearchField.TEXT]){ this.text = ''; return 1 }
            return 0
        case QtEnums.Key_Return: case QtEnums.Key_Enter:
            if(this.$popupVisible()) this.$hidePopup(true)
            engine.emit(this, SearchField.SIGNAL_accepted)
            engine.emit(this, SearchField.SIGNAL_searchTriggered)
            return 1
        case QtEnums.Key_Up:
            if(!hasModel) return 0
            if(this.$popupVisible() && highlighted > 0) this.$setHighlighted(highlighted - 1, true)
            return 1
        case QtEnums.Key_Down:
            if(!hasModel) return 0
            if(this.$popupVisible() && highlighted < count - 1) this.$setHighlighted(highlighted + 1, true)
            return 1
        case QtEnums.Key_Home:
            if(!this.$popupVisible()) return 0
            this.$setHighlighted(0, true)
            return 1
        case QtEnums.Key_End:
            if(!this.$popupVisible()) return 0
            this.$setHighlighted(count - 1, true)
            return 1
        }
        return 0
    }
    // the focus goes on to the text (QQuickSearchField::focusInEvent: Tab, Backtab, Shortcut, Other)
    $focusEvent(active, reason){
        super.$focusEvent(active, reason)
        let input = this.$field()
        if(!active || !input) return
        if(reason === 1 || reason === 2 || reason === 5 || (reason === 7 && !this.$popupVisible())) input.forceActiveFocus(reason)
    }
    $componentComplete(){
        super.$componentComplete()
        this.$countChanged()
    }
    $disposeNative(){
        if(this.$model && this.$model.$removeView) this.$model.$removeView(this)
        super.$disposeNative()
    }
}
function GroupWrite(group, id, value){ if(group.$v[id] !== value) require('../../Core/QtObject').GroupObject.prototype.$write.call(group, id, value) }
SearchField.prototype.$isFocusScope = true
defineType(SearchField, {
    props: {
        suggestionModel: 'var', delegateModel: { kind: 'var', readonly: true, initial: null }, suggestionCount: { kind: 'int', readonly: true },
        currentIndex: 'int', highlightedIndex: { kind: 'int', readonly: true }, text: 'string', textRole: 'string', live: 'bool',
        searchIndicator: { group: IndicatorButton }, clearIndicator: { group: IndicatorButton },
        popup: { kind: 'var', initial: null }, delegate: { kind: 'component', initial: null }, placeholderText: 'string',
        selectTextByMouse: 'bool', selectedText: { kind: 'string', readonly: true }, selectionStart: { kind: 'int', readonly: true },
        selectionEnd: { kind: 'int', readonly: true }, cursorPosition: 'int',
    },
    signals: ['activated(index)', 'highlighted(index)', 'accepted', 'searchTriggered', 'textEdited', 'searchButtonPressed', 'clearButtonPressed'],
})
SearchField.$setters = withSetters(Control, [
    [SearchField.SUGGESTIONMODEL, (c, v) => c.$setModel(v)],
    [SearchField.CURRENTINDEX, (c, v) => c.$plain(SearchField.CURRENTINDEX, v === undefined ? -1 : v)],
    [SearchField.POPUP, (c, v) => c.$setPopup(v)],
    [SearchField.CURSORPOSITION, (c, v) => { let i = c.$field(); if(i) i.cursorPosition = v; c.$plain(SearchField.CURSORPOSITION, i ? i.cursorPosition : v) }],
])

module.exports = { SearchField }
