const QtObject = require("../QtQml/QtObject")

class Window extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {

    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('Window')

        return obj
    }

}



module.exports = Window