const BaseObject = require("../QtBase/BaseObject")

class Signal extends BaseObject {
    /**
     * 
     * @param {Object} target 
     * @param {String} name
     * @returns {Object}
     */
    static get(target, name){
        let f = (...args)=>{
            if(target.signalsBlocked()) return

            let slotName = 'SLOT_' + name

            global.queueFlag.push(false)
            if(slotName in target){
                try {
                    target[slotName].call(target.__proxy, ...args)
                } catch (error) {
                    if(location.hash === '#jqdebug')console.error(error)
                }  
            }

            if(name in target.__connections){
                for(let connection of target.__connections[name]){
                    try {
                        if(connection.target){
                            connection.slot.call(connection.target.__proxy, ...args)  
                        } else {
                            connection.slot(...args)
                        }
                    } catch (error) {
                        if(location.hash === '#jqdebug')console.error(error)
                    }
                }
            }
            global.queueFlag.pop()
        }

        f.connect = (...args)=>{
            if(!target.__connections[name]) target.__connections[name] = []

            if(args.length === 1){
                let connection = {
                    slot:args[0]
                }

                target.__connections[name].push(connection)
                
                let connectionObj = {
                    target: target,
                    name: name,
                    connection: connection
                }

                if(connection.slot.meta && !connection.slot.meta.destruction){
                    let destructionFunc = ()=>{
                        this.removeConnection(connectionObj)
                    }
                    destructionFunc.meta = {
                        name: connection.slot.meta.name,
                        parent: connection.slot.meta.parent,
                        destruction: true
                    }
                    connection.slot.meta.parent.__proxy['Component.destruction'].connect(destructionFunc)
                }

                return connectionObj
            } else if(args.length === 2){
                let connection = {
                    target:args[0],
                    slot:args[1]
                }

                target.__connections[name].push(connection)

                let connectionObj = {
                    target: target,
                    name: name,
                    connection: connection
                }

                if(connection.slot.meta && !connection.slot.meta.destruction){
                    let destructionFunc = ()=>{
                        this.removeConnection(connectionObj)
                    }
                    destructionFunc.meta = {
                        name: connection.slot.meta.name,
                        parent: connection.slot.meta.parent,
                        destruction: true
                    }
                    connection.slot.meta.parent.__proxy['Component.destruction'].connect(destructionFunc)
                }

                return connectionObj
            }
        }

        f.disconnect = (...args)=>{
            if(!target.__connections || !target.__connections[name]) return

            let i = 0
            if(args.length === 1){
                while(i < target.__connections[name].length){
                    if(target.__connections[name][i].slot.meta && args[0].meta){
                        if(!target.__connections[name][i].target && target.__connections[name][i].slot.meta.parent === args[0].meta.parent && target.__connections[name][i].slot.meta.name === args[0].meta.name){
                            let connection = target.__connections[name].splice(i, 1)[0]

                            if(connection.slot.meta && !connection.slot.meta.destruction){
                                let destructionFunc = ()=>{
                                    this.removeConnection(connectionObj)
                                }
                                destructionFunc.meta = {
                                    name: connection.slot.meta.name,
                                    parent: connection.slot.meta.parent,
                                    destruction: true
                                }
                                connection.slot.meta.parent.__proxy['Component.destruction'].disconnect(destructionFunc)
                            }
                            
                        } else {
                            i++
                        }
                    } else {
                        if(!target.__connections[name][i].target && target.__connections[name][i].slot === args[0]){
                            let connection = target.__connections[name].splice(i, 1)[0]

                            if(connection.slot.meta && !connection.slot.meta.destruction){
                                let destructionFunc = ()=>{
                                    this.removeConnection(connectionObj)
                                }
                                destructionFunc.meta = {
                                    name: connection.slot.meta.name,
                                    parent: connection.slot.meta.parent,
                                    destruction: true
                                }
                                connection.slot.meta.parent.__proxy['Component.destruction'].disconnect(destructionFunc)
                            }
                        } else {
                            i++
                        }
                    }
                    
                }
            } else if(args.length === 2){
                while(i < target.__connections[name].length){
                    if(target.__connections[name][i].slot.meta && args[1].meta){
                        if(target.__connections[name][i].target === args[0] && target.__connections[name][i].slot.meta.target === args[1].meta.target && target.__connections[name][i].slot.meta.name === args[1].meta.name){
                            let connection = target.__connections[name].splice(i, 1)[0]

                            if(connection.slot.meta && !connection.slot.meta.destruction){
                                let destructionFunc = ()=>{
                                    this.removeConnection(connectionObj)
                                }
                                destructionFunc.meta = {
                                    name: connection.slot.meta.name,
                                    parent: connection.slot.meta.parent,
                                    destruction: true
                                }
                                connection.slot.meta.parent.__proxy['Component.destruction'].disconnect(destructionFunc)
                            }
                        } else {
                            i++
                        }
                    } else {
                        if(target.__connections[name][i].target === args[0] && target.__connections[name][i].slot === args[1]){
                            let connection = target.__connections[name].splice(i, 1)[0]

                            if(connection.slot.meta && !connection.slot.meta.destruction){
                                let destructionFunc = ()=>{
                                    this.removeConnection(connectionObj)
                                }
                                destructionFunc.meta = {
                                    name: connection.slot.meta.name,
                                    parent: connection.slot.meta.parent,
                                    destruction: true
                                }
                                connection.slot.meta.parent.__proxy['Component.destruction'].disconnect(destructionFunc)
                            }
                        } else {
                            i++
                        }
                    }
                }
            }
            if(target.__connections[name].length === 0) delete target.__connections[name]
        }

        f.extendSlot = ()=>{}

        f.meta = {
            parent: target,
            name: name
        }

        return f
    }

    /**
     */
    static set(){
        throw 'Invalid left-hand side in assignment'
    }

    static removeConnection(connectionObj){
        let target = connectionObj.target
        let name = connectionObj.name
        let connection = connectionObj.connection

        if(target.__destroyed || !target.__connections || !target.__connections[name]) return

        let index = target.__connections[name].indexOf(connection)
        if(index >= 0){
            target.__connections[name].splice(index, 1)
        }

        if(target.__connections[name].length === 0) delete target.__connections[name]
    }
}

module.exports = Signal