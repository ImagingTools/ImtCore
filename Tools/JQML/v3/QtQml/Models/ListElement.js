const QtObject = require("../../QtQml/QtObject")
const Property = require("../../QtQml/Property")


class ListElement extends QtObject {
    PROXY__get__index(){
        return this.parent.data.indexOf(this.__proxy)
    }
}



module.exports = ListElement