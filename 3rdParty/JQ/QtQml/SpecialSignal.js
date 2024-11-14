class Signal {
    static create(parent, meta){
        let f = (...args)=>{
            f.emit(...args)
        }
        f.__parent = parent ? parent.__proxy : null
        f.__slot = parent ? parent[meta.slotName] : null
        // f.__args = meta.args
        f.__proto__ = Signal.prototype
        return f
    }

    constructor(){
        throw 'Can not create Signal using constructor'
    }

    connect(...args){
        if(!this.__connections) this.__connections = []

        if(args.length === 1){
            this.__connections.push({
                slot:args[0]
            })
        } else if(args.length === 2){
            this.__connections.push({
                target:args[0],
                slot:args[1]
            })
        }
        
    }
    connectPrimary(...args){
        if(!this.__connections) this.__connections = []

        if(args.length === 1){
            this.__connections.unshift({
                slot:args[0]
            })
        } else if(args.length === 2){
            this.__connections.unshift({
                target:args[0],
                slot:args[1]
            })
        }
        
    }
    disconnect(...args){
        if(!this.__connections) return

        let i = 0
        if(args.length === 1){
            while(i < this.__connections.length){
                if(!this.__connections[i].target && this.__connections[i].slot === args[0]){
                    this.__connections.splice(i, 1)
                } else {
                    i++
                }
            }
        } else if(args.length === 2){
            while(i < this.__connections.length){
                if(this.__connections[i].target === args[0] && this.__connections[i].slot === args[1]){
                    this.__connections.splice(i, 1)
                } else {
                    i++
                }
            }
        }
        if(this.__connections.length === 0) delete this.__connections
    }

    emit(...args){
        if(this.__parent) {
            if(this.__parent.signalsBlocked()) return

            if(this.__connections)
            for(let connection of this.__connections){
                try {
                    if(connection.target){
                        connection.slot.call(connection.target, ...args)  
                    } else {
                        connection.slot(...args)
                    }
                } catch (error) {
                    console.error(error)
                }
            }

            if(this.__slot){
                this.__slot.call(this.__parent, ...args)
            }
        }
    }

    extendSlot(slot){
        let slotSuper = this.__slot
        this.__slot = (...args)=>{
            try {
                if(slotSuper) slotSuper.call(this.__parent, ...args)
                slot.call(this.__parent, ...args)
            } catch (error) {
                console.error(error)
            }
            
        }
    }

    __free(){

    }

    __destroy(){
        delete this.__connections
        delete this.__parent
        delete this.__slot
    }
}

module.exports = Signal