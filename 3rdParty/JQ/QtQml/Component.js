const QQmlComponent = require("./QQmlComponent");

class Component extends QQmlComponent {
    static create(parent, component){
        let proxy = super.create(parent)
        let self = proxy.__self 

        self.component = component

        return proxy
    } 

    createObject(parent, model){
        let obj = this.component.create(parent, model)
        
        if(!parent){
            JQApplication.MemoryController.observe(obj, this.__proxy)
        }

        return obj
    }
}

module.exports = Component