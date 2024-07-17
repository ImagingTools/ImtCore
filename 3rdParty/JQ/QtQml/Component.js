const QQmlComponent = require("./QQmlComponent");

class Component extends QQmlComponent {
    static create(parent, component, ...args){
        let proxy = super.create(parent, component, ...args)
        let self = proxy.__self 

        self.component = component

        return proxy
    } 

    createObject(parent, properties){
        return this.component.create(parent)
    }
}

module.exports = Component