const BaseObject = require("../QtBase/BaseObject")

const handler = {
    apply(proxyTarget, thisValue, args){
        let target = proxyTarget.meta.parent
        let name = proxyTarget.meta.name

        if(target.signalsBlocked() || target.__destroyed) return
            JQApplication.beginUpdate()

            let slotName = 'SLOT_' + name

            global.queueFlag.push(false)
            global.signalTargets.push(target)

            const invokeConnections = (match)=>{
                if(!(name in target.__connections)) return
                for(let connection of target.__connections[name].slice()){
                    if(!match(connection)) continue
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

            // Geometry-style listeners (e.g. Loader size sync) fire before bindings,
            // matching Qt: itemGeometryChanged runs before width/height notify.
            invokeConnections(connection => connection.priority === 2)

            // Priority connections (cross-item bindings) fire before SLOT_
            invokeConnections(connection => connection.priority === true)

            if(slotName in target){
                try {
                    target[slotName].call(target.__proxy, ...args)
                } catch (error) {
                    if(location.hash === '#jqdebug')console.error(error)
                }  
            }

            invokeConnections(connection => !connection.priority)
            global.queueFlag.pop()
            global.signalTargets.pop()
            JQApplication.endUpdate()
    },
    get(proxyTarget, key) {
        let target = proxyTarget.meta.parent
        let name = proxyTarget.meta.name

        if (key === 'meta') return proxyTarget.meta
        if (key === 'connect') return (...args)=>{
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
                        Signal.removeConnection(connectionObj)
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
                        Signal.removeConnection(connectionObj)
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
        if (key === 'connectFirst') return (...args)=>{
            if(!target.__connections[name]) target.__connections[name] = []

            if(args.length === 1){
                let connection = {
                    slot:args[0],
                    priority: 2
                }

                target.__connections[name].unshift(connection)
                
                let connectionObj = {
                    target: target,
                    name: name,
                    connection: connection
                }

                if(connection.slot.meta && !connection.slot.meta.destruction){
                    let destructionFunc = ()=>{
                        Signal.removeConnection(connectionObj)
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
        if (key === 'connectBefore') return (...args)=>{
            if(!target.__connections[name]) target.__connections[name] = []

            if(args.length === 1){
                let connection = {
                    slot:args[0],
                    priority: true
                }

                target.__connections[name].unshift(connection)
                
                let connectionObj = {
                    target: target,
                    name: name,
                    connection: connection
                }

                if(connection.slot.meta && !connection.slot.meta.destruction){
                    let destructionFunc = ()=>{
                        Signal.removeConnection(connectionObj)
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
        if (key === 'disconnect') return (...args)=>{
            if(!target.__connections || !target.__connections[name]) return

            let i = 0
            if(args.length === 1){
                while(i < target.__connections[name].length){
                    if(target.__connections[name][i].slot.meta && args[0].meta){
                        if(!target.__connections[name][i].target && target.__connections[name][i].slot.meta.parent === args[0].meta.parent && target.__connections[name][i].slot.meta.name === args[0].meta.name){
                            let connection = target.__connections[name].splice(i, 1)[0]

                            if(connection.slot.meta && !connection.slot.meta.destruction){
                                let destructionFunc = ()=>{
                                    Signal.removeConnection(connectionObj)
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
                                    Signal.removeConnection(connectionObj)
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
                                    Signal.removeConnection(connectionObj)
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
                                    Signal.removeConnection(connectionObj)
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
        if (key === 'extendSlot') return ()=>{}
    },
}

class Signal extends BaseObject {
    /**
     * 
     * @param {Object} target 
     * @param {String} name
     * @returns {Object}
     */
    static get(target, name){
        if(!target[`CACHED_SIGNAL_${name}`]){
            let f = ()=>{}
            f.meta = {
                parent: target,
                name: name
            }

            target[`CACHED_SIGNAL_${name}`] = new Proxy(f, handler)
        }

        return target[`CACHED_SIGNAL_${name}`]
        
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