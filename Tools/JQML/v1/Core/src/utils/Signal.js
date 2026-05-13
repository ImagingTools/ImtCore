export function Signal(...args){
    let names = args
    let signal = (...args) => {
        
        for(let i = 0; i < names.length; i++){
            for(let k = 0; k < names[i].length && k < args.length; k++){
                signal.context[names[i][k]] = args[k]
            }   
        }
        // let callList = []
        // for(let key in signal.connections){
        //     callList[signal.connections[key].index] = key
        // }
        
        for(let key in signal.connections){
            try {
                if(signal.connections[key]) {
                    Core.setGlobalContext(signal.context)
                    signal.connections[key].slot(...args)
                    Core.removeLastGlobalContext()
                }
            } catch (error) {
                console.error(`skip::signal::`,error)
                Core.removeLastGlobalContext()
            }
        }

        // delete signal.context
        // for(let key in signal.connections){

        //     try {
        //         if(signal.connections[key]) {
        //             Core.setGlobalContext(signal.context)
        //             signal.connections[key].slot(...args)
        //             Core.removeLastGlobalContext()
        //         }
        //     } catch (error) {
        //         console.error(`skip::signal::`,error)
        //         Core.removeLastGlobalContext()
        //     }

        // }
    }
    signal.count = 0
    signal.context = {}

    signal.connect = (...args) => {
        if(!signal.connections) signal.connections = {}

        let name = args.length === 1 ? args[0].name : args[1].name

        switch(args.length){
            case 1: signal.connections[name + signal.count] = {slot: args[0], index: signal.count}; break;
            case 2: signal.connections[name + signal.count] = {slot: args[1].bind(args[0]), index: signal.count}; break;
        }
        signal.count++
        return signal
    }
    signal.connectWithName = (name, slot) => {
        if(!signal.connections) signal.connections = {}

        signal.connections[name] = {slot: slot, index: signal.count};
        signal.count++
        return signal
    }
    signal.disconnect = (...args) => {
        if(signal.connections){
            let find = false
            for(let key in signal.connections){
                if(!find && signal.connections[key].slot === args[0]){
                    delete signal.connections[key]
                    find = true
                }
            }
            if(Object.keys(signal.connections).length === 0) delete signal.connections
        }
        // delete signal.connections[args[0].name]
    }
    signal.disconnectWithName = (name) => {
        if(signal.connections){
            delete signal.connections[name]
            if(Object.keys(signal.connections).length === 0) delete signal.connections
        }
        
    }
    return signal
}
