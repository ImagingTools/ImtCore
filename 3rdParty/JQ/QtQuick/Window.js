const QtObject = require("../QtQml/QtObject")

class Window extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {

    })

    static create(parent=null, model=null, meta={}, properties=[], isRoot=true){
        let obj = super.create(parent, model, meta, properties, isRoot)
        obj.__getDOM().classList.add('Window')

        return obj
    }

}

Window.initialize()

module.exports = Window