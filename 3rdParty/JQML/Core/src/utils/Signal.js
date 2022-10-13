export function Signal(...args){
    let names = args
    let signal = (...args) => {
        for(let i = 0; i < names.length; i++){
            for(let k = 0; k < names[i].length && k < args.length; k++){
                signal.context[names[i][k]] = args[k]
            }   
        }
        for(let key in signal.connections){

            // try {
                signal.connections[key](...args)
            // } catch (err1) {
            //     let num = 0
            //     let interval = setInterval(()=>{
            //         try {
            //             signal.connections[key](...args)
            //             clearInterval(interval)
            //         } catch (err2) {
            //             if(num > 3) {
            //                 clearInterval(interval)
                            
            //             }
            //             console.log(`ERROR(${num})::signal[${key}]`, signal.connections[key])
            //             num++
            //         }
                    
            //     }, 100 + num * 100)
            //     signal.repeats[key] = interval
            // }
            
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
        switch(args.length){
            case 1: signal.connections[args[0].name] = args[0]; break;
            case 2: signal.connections[args[1].name] = args[1].bind(args[0]); break;
        }
        return signal
    }
    signal.disconnect = (...args) => {
        delete signal.connections[args[0].name]
    }
    return signal
}
