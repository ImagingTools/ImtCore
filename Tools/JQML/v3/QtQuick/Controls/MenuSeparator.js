const Control = require("./Control")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class MenuSeparator extends Control {
    static meta = Object.assign({}, Control.meta, {
        implicitHeight: { type: Real, value: 0 },
        implicitWidth:  { type: Real, value: 0 },

        implicitHeightChanged: { type: Signal, args: [] },
        implicitWidthChanged:  { type: Signal, args: [] }
    })

    // Mark as separator so Menu's keyboard nav skips it
    __isSeparator = true

    static create(parent = null, properties = {}) {
        let obj = super.create(parent, properties)
        let dom = obj.__getDOM()
        dom.classList.add('MenuSeparator')
        obj.__setDOMStyle({
            width:       '100%',
            paddingTop:  '6px',
            paddingBottom: '6px',
            paddingLeft:   '2px',
            paddingRight:  '2px',
            boxSizing:   'border-box',
            pointerEvents: 'none',
        })

        // The visible separator line
        let line = document.createElement('div')
        line.style.cssText = 'width:100%;height:1px;background:#d0d0d0;'
        dom.appendChild(line)

        return obj
    }
}



module.exports = MenuSeparator
