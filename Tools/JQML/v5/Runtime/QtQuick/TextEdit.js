const QtEnums = require('../Qt/enums')
const { defineType, makeReadonly } = require('../Core/type')
const { Item } = require('./Item')
const { TextField, common, relayout } = require('./TextField')

class TextEdit extends TextField {
    constructor(parent = null){
        super(parent, true)
    }

    $write(id, value){
        let old = this.$v[id]
        super.$write(id, value)
        if(id === Item.WIDTH && old !== this.$v[id] && this.$v[TextEdit.WRAPMODE]) this.$updateImplicitSize()
    }

    // QQuickTextControlPrivate::setContent: a new text puts the cursor at the start
    $cursorAfterSetText(){
        return 0
    }

    // QQuickTextEditPrivate::updateMouseCursorShape (a TextInput sets no cursor)
    $cursor(){
        return this.$v[TextEdit.READONLY] && !this.$v[TextEdit.SELECTBYMOUSE] ? QtEnums.ArrowCursor : QtEnums.IBeamCursor
    }

    // QQuickTextEdit::append: a new paragraph at the end (the text itself when empty); not undone by undo() of
    // the user's edits, the cursor at the end stays at the end
    append(text){
        let content = this.$content()
        let next = content ? content + '\n' + String(text) : String(text)
        let atEnd = (p) => p === content.length ? next.length : p
        this.$recordUndo(null)
        this.$commitEdit(next, atEnd(this.$anchor()), atEnd(this.$v[TextEdit.CURSORPOSITION]), false)
    }

    // plain text: the text of the range (QQuickTextEdit::getFormattedText of a PlainText edit)
    getFormattedText(start, end){
        return this.getText(start, end)
    }

    // no links in plain text
    linkAt(){
        return ''
    }
}

defineType(TextEdit, {
    props: Object.assign({}, common, {
        wrapMode: { kind: 'int', effect: relayout },
        textFormat: 'int',
        lineCount: { kind: 'int', initial: 1, readonly: true },
        paintedWidth: { kind: 'real', readonly: true },
        paintedHeight: { kind: 'real', readonly: true },
        hoveredLink: { kind: 'string', readonly: true },
        baseUrl: 'var',
        tabStopDistance: { kind: 'real', initial: 80 },
        textMargin: 'real',
    }),
    signals: ['editingFinished', 'textEdited', 'linkActivated(link)', 'linkHovered(link)'],
})

Object.assign(TextEdit, { AlignLeft: 0x1, AlignRight: 0x2, AlignHCenter: 0x4, AlignJustify: 0x8, AlignTop: 0x20, AlignBottom: 0x40, AlignVCenter: 0x80 }, { NoWrap: 0, WordWrap: 1, WrapAnywhere: 3, Wrap: 4 }, { PlainText: 0, RichText: 1, AutoText: 2, MarkdownText: 3 })
Object.assign(TextEdit, { SelectCharacters: 0, SelectWords: 1 })

// read-only in Qt, writable in the base class
makeReadonly(TextEdit, 'implicitWidth')
makeReadonly(TextEdit, 'implicitHeight')

module.exports = { TextEdit }
