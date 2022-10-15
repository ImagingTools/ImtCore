import {Signal} from '../utils/Signal'


var context = {
    backend: {
        window: window
    },
    location: location,
    history: history,
}
context.location.pushState = (...args)=>{history.pushState(...args)}
context.location.changeHref = (href)=>{
    context.location.href = href
}

Object.defineProperty(context.location, 'searchParams', {
    get: ()=>{ 
        let res = {}
        let params = context.location.search.slice(1).split('&')
        for(let param of params){
            let temp = param.split('=')
            if(temp.length === 2) res[temp[0]] = temp[1]
        }
        return res
    },
})


export class QtObject {
    $p = {}
    $s = {}
    $ready = false
    $uL = {
        properties: [],
        aliases: [],
    }
    ID = new Set()
    LVL = new Set()

    constructor(parent) {
        this._context = context
        this.UID = UID++
        UIDList[this.UID] = this
        this.children = []
        
        if(parent){
            parent.children.push(this)
            this.parent = parent
            for(let lvl of parent.LVL){
                this.LVL.add(lvl)
            }
            
        }

        this.$s['Component.completed'] = Signal()
        this.$s['Component.destruction'] = Signal()

        // ReadyList.push(this)
        // console.log(this)

    }

    $uP(){
        this.$ready = true
        let updated = false

        while(this.$uL.properties.length){
            updated = true
            let propName = this.$uL.properties.shift()
            // caller = this.$p[propName]
            let val = this.$p[propName].func()
            // caller = null
            if(this.$p[propName].val !== val){
                this.$p[propName].val = val
                this.$p[propName].signal()
            }
            
        }
        

        for(let child of this.children){
            child.$uP()
        }

        while(this.$uL.aliases.length){
            updated = true
            let propName = this.$uL.aliases.shift()
            caller = this.$p[propName]
            let val = caller.func()
            caller = null
            if(this.$p[propName].val !== val){
                this.$p[propName].val = val
                this.$p[propName].signal()
            }
            
        }
        
        if(updated){
            this.$s['Component.completed']()// = Signal()
        }
    }

    $createReadOnlyProperty(name, func){
        Object.defineProperty(this, name, {
            get: ()=>{ 
                return func()
            },
        })
    }
    $sID(...ID){
        if(typeof ID === 'object'){
            // this.ID = new Set(ID)
            
            for(let id of ID){
                this.ID.add(id)
                IDManager.set(this, id)
            }
        } else {
            this.ID.add(ID)
            IDManager.set(this, ID)
        }
        
    }

    $cP(name, val){
        if(name in this.$p){
            return this.$p[name].signal
        }

        let signal = Signal()
        signal.debug = `${this.UID}-${name}`
        if(typeof val === 'function'){
            this.$p[name] = {
                'val': undefined,
                'signal': signal,
                'depends': new Set(),
                'func': ()=>{
                    caller = this.$p[name]
                    let res = val()
                    caller = null
                    return res
                },
                'PID': PID++
            }
            // PropertyManager.add(this.$p[name])
            this.$uL.properties.push(name)
        } else {
            this.$p[name] = {
                'val': val,
                'signal': signal,
                'depends': new Set(),
                'PID': PID++
            }
        }
        
        this.$s[`${name}Changed`] = signal
        
        Object.defineProperty(this, name, {
            get: ()=>{ 
                if(caller){
                    let _caller = caller
                    _caller.depends.add(signal)
                    signal.connections[_caller.PID] = ()=>{
                        let val = _caller.func()
                        if(_caller.val !== val){
                            _caller.val = val
                            _caller.signal()
                        }
                    }
                }
                
                return this.$p[name].val
            },
            set: (newVal)=>{
                if(this.$p[name].val !== newVal){
                    for(let s of this.$p[name].depends){
                        delete s[this.$p[name].PID]
                    }
                    this.$p[name].depends.clear()
                    this.$p[name].val = newVal
                    signal()
                }              
            },
        })
        return signal
    }

    $cPC(name, props){
        if(name in this.$p){
            return
        }
        let signal = Signal()
        signal.debug = `${this.UID}-${name}`

        this.$s[`${name}Changed`] = signal

        this[name] = {

        }

        for(let name2 in props){
            
            this.$p[`${name}.${name2}`] = {
                'val': props[name2],
                'signal': signal,
                'depends': new Set(),
                'PID': PID++,
            }
            Object.defineProperty(this[name], name2, {
                get: ()=>{ 
                    if(caller){
                        let _caller = caller
                        _caller.depends.add(signal)
                        signal.connections[_caller.PID] = ()=>{
                            let val = _caller.func()
                            if(_caller.val !== val){
                                _caller.val = val
                                _caller.signal()
                            }
                        }
                    }

                    return this.$p[`${name}.${name2}`].val
                },
                set: (newVal)=>{
                    if(this.$p[`${name}.${name2}`].val !== newVal){
                        for(let s of this.$p[`${name}.${name2}`].depends){
                            delete s[this.$p[`${name}.${name2}`].PID]
                        }
                        this.$p[`${name}.${name2}`].depends.clear()
                        this.$p[`${name}.${name2}`].val = newVal
                        signal()
                    }
                },
            })
        }
        return signal
    }

    $sP(name, func){
        // let names = name.split('.')
        
        // if(names.length === 1){
        //     if(!(names[0] in this.$p)){
        //         this.$cP(name, 'var', undefined)
        //     }
        // } else if(names.length === 2){
        //     if(!(names[0] in this.$p) && !(names[1] in this.$p[names[0]])){
        //         this.$cP(name, 'var', undefined)
        //     }
        // }

        if(this.$p[name]){
            this.$p[name].func = ()=>{
                caller = this.$p[name]
                let res = func()
                caller = null
                return res
            }
            this.$uL.properties.push(name)
        } else {
            this.$cP(name, func)
        }
        
    }

    $cA(name, getter, setter){
        let signal = Signal()

        this.$p[name] = {
            'val': '',
            'signal': signal,
            'depends': new Set(),
            'func': ()=>{
                caller = this.$p[name]
                let res = getter()
                caller = null
                return res
            },
            'PID': PID++
        }
        this.$s[`${name}Changed`] = signal
        
        Object.defineProperty(this, name, {
            get: ()=>{ 
                if(caller){
                    let _caller = caller
                    _caller.depends.add(signal)
                    signal.connections[_caller.PID] = ()=>{
                        let val = _caller.func()
                        if(_caller.val !== val){
                            _caller.val = val
                            _caller.signal()
                        }
                    }
                }
                
                return this.$p[name].val
            },
            set: (newVal)=>{
                setter(newVal)           
            },
        })

        // PropertyManager.add(this.$p[name])
        this.$uL.aliases.push(name)
        return signal

        // let signal = Signal()
        // this.$p[name] = {
        //     'signal': signal,
        //     'depends': [],
            
        // }
        // this.$s[`${name}Changed`] = signal
        
        // Object.defineProperty(this, name, {
        //     get: ()=>{ 
        //         caller.obj = this
        //         caller.prop = name

        //         this.$p[name].depend = []
        //         let val = getter()

        //         caller.obj = null
        //         caller.prop = ''

        //         return val
        //     },
        //     set: (newVal)=>{
        //         let oldVal = getter()
        //         if(oldVal !== newVal){
        //             setter(newVal)
        //             signal()
        //         }              
        //     },
        // }) 
    }

    $cS(name, ...args){
        let signal = Signal(args)
        this.$s[name] = signal
        this[name] = signal
    }

    $updateGeometry(){
        if(this.parent) this.parent.$updateGeometry()
    }
    
    $domCreate(){
        
    }
    $updateRect(){
        
    }

    $setDom(dom){
        this.dom = dom
    }

    $destroy(){
        delete UIDList[this.UID]

        this.$uL.properties = []
        this.$uL.aliases = []

        if(this.ID.size){
            IDManager.remove(this)
        }
        for(let child of this.children){
            child.parent = null
            child.$destroy()
        }
        this.children = []
        if(this.parent){
            let indx = this.parent.children.indexOf(this)
            if(indx >= 0) this.parent.children.splice(indx, 1)

        }
        for(let propName in this.$p){
            
            for(let s of this.$p[propName].depends){
                // delete s[this.$p[propName]]
                delete s.connections[this.$p[propName].PID]
            }
            this.$p[propName].depends.clear()
            this.$p[propName].signal.connections = {}      
        }
        for(let sigName in this.$s){
            this.$s[sigName].connections = {}
        }

        this.$s['Component.destruction']()

        for(let key in this){
            this[key] = null
        }
    }

    createComponent(_componentPath, _parent){
        return Core.createComponent(_componentPath, _parent)
    }
    
}


QML.QtObject = QtObject
