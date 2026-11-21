const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Dialog } = require('./Dialog')

// ColorDialog of QtQuick.Dialogs (QQuickColorDialog, Qt 6.12). On the web the color input of the browser picks the
// color: the dialog is accepted with it, rejected when it is closed without one (its alpha is the one of selectedColor,
// as the input has none)
class ColorDialog extends Dialog {
    $show(){
        if(typeof document === 'undefined') return
        let input = document.createElement('input')
        input.type = 'color'
        let color = String(this.$v[ColorDialog.SELECTEDCOLOR] || '#ffffff')
        let alpha = color.length === 9 ? color.slice(1, 3) : ''
        input.value = '#' + color.slice(color.length === 9 ? 3 : 1, color.length === 9 ? 9 : 7)
        input.style.cssText = 'position:fixed;left:0;top:0;width:0;height:0;opacity:0;border:0;padding:0'
        let done = false
        let finish = (accepted) => engine.batch(() => {
            if(done) return
            done = true
            input.remove()
            if(!accepted){
                this.reject()
                return
            }
            this.selectedColor = alpha && alpha !== 'ff' ? '#' + alpha + input.value.slice(1) : input.value
            this.accept()
        })
        input.addEventListener('change', () => finish(true))
        input.addEventListener('cancel', () => finish(false))
        document.body.appendChild(input)
        input.click()
    }
}
defineType(ColorDialog, {
    props: {
        selectedColor: { kind: 'color', initial: '#ffffff' },
    },
})
Object.assign(ColorDialog, { ShowAlphaChannel: 0x1, NoButtons: 0x2, DontUseNativeDialog: 0x4, NoEyeDropperButton: 0x8, Accepted: 1, Rejected: 0 })

module.exports = { ColorDialog }
