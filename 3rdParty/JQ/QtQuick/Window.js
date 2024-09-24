const QtObject = require("../QtQml/QtObject")

class Window extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {

    })

    static create(parent, model, properties=[], ...args){
        let proxy = super.create(parent, model, properties, ...args)
        proxy.__getDOM().classList.add('Window')

        return proxy
    }

}

module.exports = Window