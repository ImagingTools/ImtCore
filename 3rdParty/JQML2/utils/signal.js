class QSignal {
    constructor(...args){
        let f = (...args)=>{
            f.emit(...args)
        }
        f.args = args
        f.__proto__ = QSignal.prototype
        return f
    }
    connect(...args){
        if(!this.connections) this.connections = []

        if(args.length === 1){
            this.connections.push({
                slot:args[0]
            })
        } else if(args.length === 2){
            this.connections.push({
                target:args[0],
                slot:args[1]
            })
        }
        
    }
    connectPrimary(...args){
        if(!this.connections) this.connections = []

        if(args.length === 1){
            this.connections.unshift({
                slot:args[0]
            })
        } else if(args.length === 2){
            this.connections.unshift({
                target:args[0],
                slot:args[1]
            })
        }
        
    }
    disconnect(...args){
        if(!this.connections) return

        if(args.length === 1){
            for(let i = 0; i < this.connections.length; i++){
                if(this.connections[i].slot === args[0]){
                    this.connections.splice(i, 1)
                    if(this.connections.length === 0) delete this.connections
                    return
                }
            }
        } else if(args.length === 2){
            for(let i = 0; i < this.connections.length; i++){
                if(this.connections[i].target === args[0] && this.connections[i].slot === args[1]){
                    this.connections.splice(i, 1)
                    if(this.connections.length === 0) delete this.connections
                    return
                }
            }
        }
    }

    emit(...args){
        if(!this.connections) return

        for(let connection of this.connections){
            if(connection.target){
                connection.slot.call(connection.target, ...args)  
            } else {
                connection.slot(...args)
            }
        }
    }

    destroy(){
        delete this.connections
    }
}

module.exports.QSignal = QSignal
