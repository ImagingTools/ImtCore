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

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)

        window.addEventListener('load', ()=>{
            obj.width = window.innerWidth
            obj.height = window.innerHeight
        })

        window.addEventListener('resize', ()=>{
            obj.width = window.innerWidth
            obj.height = window.innerHeight
        })

        return obj
    }
}



module.exports = Screen.create()