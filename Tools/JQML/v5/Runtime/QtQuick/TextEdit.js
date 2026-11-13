const { defineType, makeReadonly } = require('../Core/type')
const { Item } = require('./Item')
const { TextField, common } = require('./TextField')

class TextEdit extends TextField {
    constructor(parent = null){
        super(parent, true)
    }

    $write(id, value){
        let old = this.$v[id]
        super.$write(id, value)
        if(id === Item.WIDTH && old !== this.$v[id] && this.$v[TextEdit.WRAPMODE]) this.$updateImplicitSize()
    }
}

defineType(TextEdit, {
    props: Object.assign({}, common, {
        wrapMode: { kind: 'int', effect: (edit) => edit.$updateImplicitSize() },
        textFormat: 'int',
        lineCount: { kind: 'int', initial: 1, readonly: true },
        paintedWidth: { kind: 'real', readonly: true },
        paintedHeight: { kind: 'real', readonly: true },
    }),
    signals: ['editingFinished', 'textEdited', 'linkActivated(link)'],
})

Object.assign(TextEdit, { AlignLeft: 0x1, AlignRight: 0x2, AlignHCenter: 0x4, AlignJustify: 0x8, AlignTop: 0x20, AlignBottom: 0x40, AlignVCenter: 0x80 }, { NoWrap: 0, WordWrap: 1, WrapAnywhere: 3, Wrap: 4 }, { PlainText: 0, RichText: 1, AutoText: 2, MarkdownText: 3 })

// read-only in Qt, writable in the base class
makeReadonly(TextEdit, 'implicitWidth')
makeReadonly(TextEdit, 'implicitHeight')

module.exports = { TextEdit }
