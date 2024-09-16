const Property = require("../Property")
const Int = require("../Int")
const Signal = require("../Signal")
const JQApplication = require("../../core/JQApplication")


class AbstractItemModel {
    static create(parent, index, data){
        let model = Object.assign({
            index: index,
            __beginUpdate: ()=>{
                parent.__beginUpdate()
            },
            __endUpdate: ()=>{
                parent.__endUpdate()
            },
            __destroy: ()=>{
                
            }
        }, data ? data : {})

        return new Proxy(model, { 
            has(target, key){
                return key in target
            },
            get(target, key){
                return target[key]
            },
            set(target, key, value){
                JQApplication.updateLater(model)
                target[key] = value
                return true
            }
        })
    }
}

module.exports = AbstractItemModel