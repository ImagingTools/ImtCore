const QQmlComponent = require("./QQmlComponent");

class Component extends QQmlComponent {
    static create(parent, component){
        let proxy = super.create(parent)
        let self = proxy.__self 

        self.component = component

        return proxy
    } 

    createObject(parent, model){
        return this.component.create(parent, model)
    }
}

module.exports = Component