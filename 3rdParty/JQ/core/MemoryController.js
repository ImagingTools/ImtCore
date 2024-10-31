const Property = require("../QtQml/Property")

module.exports = {
    links: new Map(),

    addLink: function(target, source){
        if(target && typeof target === 'object' && source instanceof Property){
            if(this.links.has(target)){
                let list = this.links.get(target)
                list.push(source)
            } else {
                this.links.set(target, [source])
            }
        }
    },

    removeLink: function(target, source){
        if(target && typeof target === 'object' && source instanceof Property){
            if(this.links.has(target)){
                let list = this.links.get(target)
                let index = list.indexOf(source)

                if(index >= 0){
                    list.splice(index, 1)
                }
            }
        }
    },

    delete: function(target){
        if(target && typeof target === 'object'){
            if(this.links.has(target)){
                let list = this.links.get(target)
                for(let property of list){
                    property.__value = null
                }
                this.links.delete(target)
            }
        }
    }
}