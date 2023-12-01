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
            args[0].setDependsSignal(this)
        } else if(args.length === 2){
            this.connections.push({
                target:args[0],
                slot:args[1]
            })
            args[1].setDependsSignal(this)
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
                    args[0].removeDependsSignal(this)
                    if(this.connections.length === 0) delete this.connections
                    return
                }
            }
        } else if(args.length === 2){
            for(let i = 0; i < this.connections.length; i++){
                if(this.connections[i].target === args[0] && this.connections[i].slot === args[1]){
                    this.connections.splice(i, 1)
                    args[1].removeDependsSignal(this)
                    if(this.connections.length === 0) delete this.connections
                    return
                }
            }
        }
    }

    emit(...args){
        if(!this.connections) return

        for(let connection of this.connections){
            // try {
                if(connection.target){
                    connection.slot.call(connection.target, ...args)  
                } else {
                    connection.slot(...args)
                }
            // } catch (error) {
                
            // }
            
        }
    }

    setDependsSignal(signal){
        if(!this.depends) this.depends = []
        if(this.depends.indexOf(signal) < 0) this.depends.push(signal)
    }

    removeDependsSignal(signal){
        let index = this.depends.indexOf(signal)
        if(index >= 0) this.depends.splice(index, 1)
        if(this.depends.length === 0) delete this.depends
    }
    clearDependsSignal(){
        while(this.depends && this.depends.length){
            let signal = this.depends.pop()
            signal.disconnect(this)
        }
        delete this.depends
    }

    destroy(){
        this.clearDependsSignal()
        delete this.connections
    }
}

module.exports.QSignal = QSignal
