export function Signal(...args){
    let names = args
    let signal = (...args) => {

        for(let i = 0; i < names.length; i++){
            for(let k = 0; k < names[i].length && k < args.length; k++){
                signal.context[names[i][k]] = args[k]
            }   
        }
        for(let key in signal.connections){

            try {
                if(signal.connections[key]) signal.connections[key](...args)
            } catch (err1) {
                // signal.repeats[key] = setTimeout(()=>{
                //     signal.connections[key](...args)
                // }, 500)
            }
            
            // let interval = setInterval(()=>{
            //     try{
            //         signal.connections[key](...args)
            //         clearInterval(interval)
            //     } catch {
            //         console.log('ERROR::', key, signal.connections[key])
            //         // signal.connections[key](...args)
            //     }
            // }, 30)  
        }
    }
    signal.repeats = {}
    signal.context = {}
    signal.connections = {}
    signal.connect = (...args) => {
        let name = args.length === 1 ? args[0].name : args[1].name
        let count = 0
        while(signal.connections[name + count]){
            count++
        }
        switch(args.length){
            case 1: signal.connections[name + count] = args[0]; break;
            case 2: signal.connections[name + count] = args[1].bind(args[0]); break;
        }
        return signal
    }
    signal.disconnect = (...args) => {
        let find = false
        for(let key in signal.connections){
            if(!find && signal.connections[key] === args[0]){
                delete signal.connections[key]
                find = true
            }
        }
        // delete signal.connections[args[0].name]
    }
    return signal
}
