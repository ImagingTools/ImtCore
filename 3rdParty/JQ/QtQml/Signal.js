class Signal {
    static create(parent, meta){
        let f = (...args)=>{
            f.emit(...args)
        }
        f.__parent = parent.__proxy
        f.__slotName = meta.slotName
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

        if(args.length === 1){
            for(let i = 0; i < this.__connections.length; i++){
                if(this.__connections[i].slot === args[0]){
                    this.__connections.splice(i, 1)
                    if(this.__connections.length === 0) delete this.__connections
                    return
                }
            }
        } else if(args.length === 2){
            for(let i = 0; i < this.__connections.length; i++){
                if(this.__connections[i].target === args[0] && this.__connections[i].slot === args[1]){
                    this.__connections.splice(i, 1)
                    if(this.__connections.length === 0) delete this.__connections
                    return
                }
            }
        }
    }

    emit(...args){
        if(this.__parent) {
            if(this.__parent.signalsBlocked()) return

            if(this.__parent[this.__slotName]){
                this.__parent[this.__slotName](...args)
            }
        }

        if(!this.__connections) return

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
    }

    __free(){

    }

    __destroy(){
        delete this.__connections
        // delete this.args
        delete this.parent
        delete this.slotName
    }
}

module.exports = Signal