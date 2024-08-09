const QObject = require("./QObject")
const Real = require("./Real")
const Signal = require("./Signal")

class Screen extends QObject {
    static meta = Object.assign({}, QObject.meta, {
        width: {type:Real, value:0, signalName:'widthChanged'},
        height: {type:Real, value:0, signalName:'heightChanged'},

        widthChanged: {type:Signal, slotName:'onWidthChanged', args:[]},
        heightChanged: {type:Signal, slotName:'onHeightChanged', args:[]},
    })

    static create(parent, model, ...args){
        let proxy = super.create(parent, model, ...args)

        window.addEventListener('load', ()=>{
            proxy.width = window.innerWidth
            proxy.height = window.innerHeight
        })

        window.addEventListener('resize', ()=>{
            proxy.width = window.innerWidth
            proxy.height = window.innerHeight
        })

        return proxy
    }
}

module.exports = Screen.create()