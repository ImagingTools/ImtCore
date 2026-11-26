const engine = require('../../Core/engine')
const { QtObject } = require('../../Core/QtObject')
const { defineType } = require('../../Core/type')

class Dialog extends QtObject {
    open(){ this.visible = true }
    close(){ this.visible = false }
    accept(){
        this.$write(Dialog.RESULT, 1)
        this.visible = false
        engine.emitArgs(this, Dialog.SIGNAL_accepted, [])
    }
    reject(){
        this.$write(Dialog.RESULT, 0)
        this.visible = false
        engine.emitArgs(this, Dialog.SIGNAL_rejected, [])
    }
}
defineType(Dialog, {
    props: {
        title: 'string',
        visible: { kind: 'bool', effect(dialog, value){ if(value) dialog.$show() } },
        modality: { kind: 'int', initial: 1 },
        flags: 'int',
        result: 'int',
        parentWindow: 'var',
        acceptLabel: 'string',
        rejectLabel: 'string',
        options: 'int',
    },
    signals: ['accepted', 'rejected'],
})
Dialog.prototype.$show = function(){}
Object.assign(Dialog, { Accepted: 1, Rejected: 0 })

module.exports = { Dialog }
