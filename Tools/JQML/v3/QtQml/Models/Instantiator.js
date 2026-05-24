const QObject = require("../../QtQml/QObject")
const Component = require("../../QtQml/Component")
const QtObject = require("../../QtQml/QtObject")
const Bool = require("../../QtQml/Bool")
const Int = require("../../QtQml/Int")
const Var = require("../../QtQml/Var")
const Variant = require("../../QtQml/Variant")
const Signal = require("../../QtQml/Signal")


class Instantiator extends QObject {
    static meta = Object.assign({}, QObject.meta, {
        active: {type:Bool, value:true, },
        asynchronous: {type:Bool, value:false, },
        count: {type:Int, value:0, },
        delegate: {type:Variant, typeTarget:Component, value:undefined, },
     
        model: {type:Var, value:undefined, },
        object: {type:Variant, typeTarget:QtObject, value:undefined, },

        activeChanged: {type:Signal, args:[]},
        asynchronousChanged: {type:Signal, args:[]},
        countChanged: {type:Signal, args:[]},
        delegateChanged: {type:Signal, args:[]},
        modelChanged: {type:Signal, args:[]},
        objectChanged: {type:Signal, args:[]},

    })
}


module.exports = Instantiator