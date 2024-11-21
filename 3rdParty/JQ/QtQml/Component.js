const QQmlComponent = require("./QQmlComponent");

class Component extends QQmlComponent {
    static create(parent, component){
        let obj = super.create(parent)

        obj.__component = component
        obj.__component.initialize()

        return obj
    } 

    createObject(parent, model){
        return this.__component.create(parent, model)
    }
}

Component.initialize()

module.exports = Component