const Property = require("../QtQml/Property")
const List = require("../QtQml/List")
const QObject = require("../QtQml/QObject")

module.exports = {
    objects: new Map(),

    observe: function(target){
        if(!this.objects.has(target)) this.objects.set(target, [])
    },

    addLink: function(target, source){
        if(target instanceof QObject && source instanceof Property){
            let parent = source.__parent
            while(parent){
                if(target === parent) return
                parent = parent.parent
            }
            
            if(this.objects.has(target)){
                let list = this.objects.get(target)
                list.push(source)
            }
        }
    },

    removeLink: function(target, source){
        if(target instanceof QObject && source instanceof Property){
            if(this.objects.has(target)){
                let list = this.objects.get(target)
                let index = list.indexOf(source)

                if(index >= 0){
                    list.splice(index, 1)
                }

                if(list.length === 0){
                    target.destroy()
                }
            }
        }
    },

    delete: function(target){
        if(target instanceof QObject){
            if(this.objects.has(target)){
                let list = this.objects.get(target)
                for(let property of list){
                    if(property instanceof List){
                        property.__removeObject(target)
                    } else {
                        property.__value = null
                    }
                    
                }
                this.objects.delete(target)
            }
        }
    }
}