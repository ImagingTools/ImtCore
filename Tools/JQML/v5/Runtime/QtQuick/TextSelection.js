const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { makeFont } = require('../Core/ValueTypes')
const { parseColor, colorName } = require('../Core/color')
const RT = require('./TextDocument/RichText')
const F = require('./TextDocument/Formats')
const L = require('./TextDocument/Layout')
const { TextCursor, MOVE } = require('./TextDocument/Cursor')
const { fragmentOf, insertFragment } = require('./TextDocument/Fragment')

// TextSelection (QQuickTextSelection of Qt 6.12): a range of a TextEdit's document read and changed through a
// QTextCursor. TextEdit.cursorSelection is the TextEdit's own cursor (its selection); a TextSelection declared in QML
// has a cursor of its own over the document of the TextEdit it is in (or of the document it is given)
class TextSelection extends QtObject {
    constructor(parent = null, control = false){
        super(parent)
        // the TextEdit whose cursor this is (cursorSelection), else null
        this.$control = control ? parent : null
        this.$anchorPos = 0
        this.$positionPos = 0
        this.$syncing = false
        engine.scheduleComponentComplete(this)
    }

    $componentComplete(){
        if(super.$componentComplete) super.$componentComplete()
        // a TextSelection in a TextEdit is over its document
        if(!this.$control && !this.$v[TextSelection.DOCUMENT] && this.$parent && this.$parent.textDocument) this.$write(TextSelection.DOCUMENT, this.$parent.textDocument)
        this.$sync()
    }

    // the TextEdit of the document
    $edit(){
        if(this.$control) return this.$control
        let document = this.$v[TextSelection.DOCUMENT]
        return document ? document.$edit : null
    }

    // the cursor (a copy): the TextEdit's for cursorSelection, the own one else
    $cursor(){
        let edit = this.$edit()
        if(!edit) return null
        let doc = edit.$doc
        let max = doc.length()
        let cursor = new TextCursor(doc, 0)
        if(this.$control){
            cursor.setPosition(edit.$anchor())
            cursor.setPosition(edit.$v[edit.constructor.CURSORPOSITION], true)
        } else {
            cursor.setPosition(Math.min(this.$anchorPos, max))
            cursor.setPosition(Math.min(this.$positionPos, max), true)
        }
        return cursor
    }

    // the cursor moved: the TextEdit's selection (QQuickTextControl::setTextCursor), or the own one
    $setCursor(anchor, position){
        if(this.$control){
            this.$control.$applySelection(anchor, position, true)
        } else {
            this.$anchorPos = anchor
            this.$positionPos = position
        }
        this.$sync()
    }

    // an edit of the document through a copy of the cursor: the TextEdit's text follows. moves: the cursor goes where the
    // edit left the copy (an edit of the text); a change of formats leaves the cursor as it is
    $editDocument(cursor, change, moves = false){
        let edit = this.$edit()
        edit.$beginEdit()
        change(cursor)
        RT.updateContentDirection(edit.$doc)
        if(this.$control && moves){
            // the control's cursor where the edit left it (QTextCursor of the control adjusted)
            edit.$commitEdit(edit.$doc, cursor.anchor, cursor.position, false)
        } else if(this.$control){
            edit.$commitEdit(edit.$doc, edit.$anchor(), edit.$v[edit.constructor.CURSORPOSITION], false)
        } else {
            let anchor = edit.$anchor(), position = edit.$v[edit.constructor.CURSORPOSITION]
            let max = edit.$doc.length()
            edit.$commitEdit(edit.$doc, Math.min(anchor, max), Math.min(position, max), false)
            this.$anchorPos = cursor.anchor
            this.$positionPos = cursor.position
        }
        if(!edit.$rich) edit.$styled = true
        this.$sync()
    }

    // the properties as the cursor has them now
    $sync(){
        let C = TextSelection
        let cursor = this.$cursor()
        if(!cursor) return
        let edit = this.$edit()
        let format = cursor.charFormat()
        let base = edit.$fontInfo()
        let font = L.resolveFont(format, base)
        let size = 'sizeAdjustment' in format || 'pixelSize' in format || base.sizeUnit === 'px' && !('pointSize' in format)
            ? { pixelSize: Math.round(font.pixelSize) } : { pointSize: 'pointSize' in format ? format.pointSize : base.pointSize }
        let fontValue = makeFont(Object.assign({ family: font.family, weight: font.weight, bold: font.weight > 500, italic: font.italic,
            underline: font.underline, strikeout: font.strikeout, capitalization: font.capitalization, letterSpacing: font.letterSpacing,
            wordSpacing: font.wordSpacing }, size))
        let block = cursor.blockFormat()
        let alignment = block.alignment || 0
        if(!(alignment & 0xf)) alignment |= F.ALIGN_LEFT
        this.$syncing = true
        try {
            this.$write(C.SELECTIONSTART, cursor.selectionStart())
            this.$write(C.SELECTIONEND, cursor.selectionEnd())
            this.$write(C.TEXT, cursor.selectedText())
            let old = this.$v[C.FONT]
            if(!old || old.toString() !== fontValue.toString()) this.$write(C.FONT, fontValue)
            this.$write(C.COLOR, format.foreground ? format.foreground : '#000000')
            this.$write(C.ALIGNMENT, alignment)
        } finally {
            this.$syncing = false
        }
    }

    $write(id, value){
        let C = TextSelection
        if(this.$syncing || id === C.DOCUMENT) return super.$write(id, value)
        let cursor = this.$cursor()
        if(!cursor) return super.$write(id, value)
        switch(id){
            case C.SELECTIONSTART: {
                // QQuickTextSelection::setSelectionStart: the cursor there (MoveAnchor)
                value = Math.trunc(Number(value))
                if(value === cursor.selectionStart()) return
                cursor.setPosition(value)
                return this.$setCursor(cursor.anchor, cursor.position)
            }
            case C.SELECTIONEND: {
                value = Math.trunc(Number(value))
                if(value === cursor.selectionEnd()) return
                cursor.setPosition(value, true)
                return this.$setCursor(cursor.anchor, cursor.position)
            }
            case C.TEXT: {
                value = value === undefined || value === null ? '' : String(value)
                if(cursor.selectedText() === value) return
                return this.$editDocument(cursor, (c) => c.insertText(value), true)
            }
            case C.FONT: return this.$setFont(cursor, value)
            case C.COLOR: {
                let color = parseColor(String(value))
                let name = color ? colorName(color) : '#000000'
                if(!cursor.hasSelection()) cursor.selectWordUnderCursor()
                let current = cursor.charFormat().foreground || '#000000'
                if(current === name) return
                return this.$editDocument(cursor, (c) => c.mergeCharFormat({ foreground: name }))
            }
            case C.ALIGNMENT: {
                value = Math.trunc(Number(value)) || 0
                if(value === this.$v[C.ALIGNMENT]) return
                return this.$editDocument(cursor, (c) => c.mergeBlockFormat({ alignment: value }))
            }
        }
        return super.$write(id, value)
    }

    // QQuickTextSelection::setFont: the font's properties merged into the selection (the word under the cursor without one)
    $setFont(cursor, value){
        if(!value || typeof value !== 'object') return
        if(!cursor.hasSelection()) cursor.selectWordUnderCursor()
        let pixel = value.$pixel || (Object.prototype.hasOwnProperty.call(value, 'pixelSize') && !Object.prototype.hasOwnProperty.call(value, 'pointSize'))
        let weight = value.weight !== undefined ? value.weight : (value.bold ? 700 : 400)
        if(value.bold === true && weight < 700) weight = 700
        if(value.bold === false && weight > 500) weight = 400
        // QTextCharFormat::setFont (FontPropertiesAll)
        let format = {
            families: [value.family || ''],
            weight,
            italic: !!value.italic,
            underlineStyle: value.underline ? F.SINGLE_UNDERLINE : F.NO_UNDERLINE,
            fontUnderline: !!value.underline,
            overline: false,
            strikeOut: !!value.strikeout,
            capitalization: value.capitalization || 0,
            wordSpacing: value.wordSpacing || 0,
            letterSpacingType: value.letterSpacing ? 'absolute' : 'percent',
            letterSpacing: value.letterSpacing || 0,
            kerning: value.kerning !== false,
        }
        if(pixel) format.pixelSize = value.pixelSize
        else format.pointSize = value.pointSize
        this.$editDocument(cursor, (c) => c.mergeCharFormat(format))
    }

    // QQuickTextSelection::moveSelectionStart: the cursor moved n times by op, nothing selected
    moveSelectionStart(op, n = 1){
        return this.$move(op, n, false)
    }

    // QQuickTextSelection::moveSelectionEnd: the cursor moved, the anchor stays
    moveSelectionEnd(op, n = 1){
        return this.$move(op, n, true)
    }

    $move(op, n, keepAnchor){
        let cursor = this.$cursor()
        if(!cursor) return false
        let edit = this.$edit()
        let moved = cursor.movePosition(Number(op), keepAnchor, Math.max(1, Math.trunc(n)), edit.$lineMoves ? edit.$lineMoves() : null)
        if(moved || this.$control) this.$setCursor(cursor.anchor, cursor.position)
        return moved
    }

    // QQuickTextSelection::duplicate: a copy of the selection after it, the copy selected
    duplicate(){
        let cursor = this.$cursor()
        if(!cursor) return
        let fragment = fragmentOf(cursor.doc, cursor.selectionStart(), cursor.selectionEnd())
        let start = cursor.selectionEnd()
        let at = new TextCursor(cursor.doc, start)
        this.$editDocument(at, (c) => insertFragment(c, fragment), true)
        let end = at.position
        this.$setCursor(start, end)
    }

    // QQuickTextSelection::linkTo: the selection (the word under the cursor without one) a link to destination
    linkTo(destination){
        let cursor = this.$cursor()
        if(!cursor) return
        if(!cursor.hasSelection()) cursor.selectWordUnderCursor()
        let format = F.with_(cursor.charFormat(), { foreground: '#0000ff', isAnchor: true, anchorHref: String(destination) })
        this.$editDocument(cursor, (c) => c.setCharFormat(format))
    }
}

// the font is a value: a write to one of its properties is a write of the whole font (QML value type write-back)
function fontGetter(){
    let id = TextSelection.FONT
    if(this.$pending) engine.flushPending(this, id)
    engine.track(this, TextSelection.SIGNAL_fontChanged)
    let font = this.$v[id]
    if(!font) return font
    let selection = this
    return new Proxy(font, {
        set(target, key, value){
            let copy = makeFont(Object.assign({}, fontObject(target), { [key]: value }))
            if(key === 'pixelSize') Object.defineProperty(copy, '$pixel', { value: true })
            else if(key !== 'pointSize' && target.$pixel) Object.defineProperty(copy, '$pixel', { value: true })
            // QFont::bold is the weight above QFont::Medium
            if(key === 'bold') copy.weight = value ? 700 : 400
            else copy.bold = copy.weight > 500
            selection.font = copy
            return true
        },
    })
}

function fontObject(font){
    let keys = ['family', 'pointSize', 'pixelSize', 'weight', 'italic', 'underline', 'strikeout', 'capitalization', 'letterSpacing', 'wordSpacing', 'kerning']
    let out = {}
    for(let key of keys) out[key] = font[key]
    if(!font.$pixel) delete out.pixelSize
    else delete out.pointSize
    return out
}

defineType(TextSelection, {
    props: {
        document: 'var',
        selectionStart: 'int',
        selectionEnd: 'int',
        text: 'string',
        font: { kind: 'var', get: fontGetter },
        color: 'color',
        alignment: 'int',
    },
})

// QQuickTextSelection::MoveOperation (QTextCursor::MoveOperation)
Object.assign(TextSelection, MOVE)

module.exports = { TextSelection }
