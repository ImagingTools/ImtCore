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
    $completed = false
    $uL = {
        properties: [],
        aliases: [],
    }
    context = {}
    ID = new Set()
    LVL = new Set()

    constructor(args) {
        this._context = context
        this.UID = UID++
        UIDList[this.UID] = this
        this.children = []
        this.$treeChilds = []
        this.$treeParent = null
        this.$treeParent2 = null
        
        
        if('repeater' in args){
            this.$repeater = args.repeater
            delete args['repeater']
        }
        if('parent' in args && args.parent !== null){
            this.$cP('index', args.parent.index)
            this.index = args.parent.index

            args.parent.$treeChilds.push(this)
            args.parent.children.push(this)
            args.parent.$childChanged()
            this.parent = args.parent
            this.$treeParent = args.parent 
        }
        if('index' in args){
            this.$cP('index', args.index)
            this.index = args.index
        }

        this.$s['Component.completed'] = Signal()
        this.$s['Component.destruction'] = Signal()

        // ReadyList.push(this)
        // console.log(this)
        Object.defineProperty(this, 'model', {
            get: ()=>{
                if(this.$repeater){
                    if(typeof this.$repeater.$p.model.val === 'number'){     
                        return {
                            'index': this.index
                        }
                    } else if (typeof this.$repeater.$p.model.val === 'object'){
                        if(Array.isArray(this.$repeater.$p.model.val)){
                            return this.$repeater.$p.model.val[this.index] ? this.$repeater.$p.model.val[this.index] : {}
                        } else {
                            return this.$repeater.$p.model.val.data[this.index] ? this.$repeater.$p.model.val.data[this.index] : {}
                        }
                    }
                }
                let parent = this.parent
                while(parent){
                    
                    if(parent.$repeater){
                        if(typeof parent.$repeater.$p.model.val === 'number'){     
                            return {
                                'index': this.index
                            }
                        } else if (typeof parent.$repeater.$p.model.val === 'object'){
                            if(Array.isArray(parent.$repeater.$p.model.val)){
                                return parent.$repeater.$p.model.val[this.index] ? parent.$repeater.$p.model.val[this.index] : {}
                            } else {
                                return parent.$repeater.$p.model.val.data[this.index] ? parent.$repeater.$p.model.val.data[this.index] : {}
                            }
                            
                        }
                    }
                    if(parent.$useModel) {
                        if(typeof parent.$p.model.val === 'number'){     
                            return {
                                'index': this.index
                            }
                        } else if (typeof parent.$p.model.val === 'object'){
                            if(Array.isArray(parent.$p.model.val)){
                                return parent.$p.model.val[this.index] ? parent.$p.model.val[this.index] : {}
                            } else {
                                return parent.$p.model.val.data[this.index] ? parent.$p.model.val.data[this.index] : {}
                            }
                            
                        }
                        
                    }
                    parent = parent.parent
                }
            },
            configurable: true,
        })
    }
    $childChanged(){
        
    }
    $tryComplete(){
        if(!this.$completed){
            this.$completed = true
            this.$s['Component.completed']()
        }

        for(let i = this.children.length-1; i >= 0; i--){
            this.children[i].$tryComplete()
        }
        // if(this.$uL.properties.length + this.$uL.aliases.length === 0) {
        //     this.$s['Component.completed']()
        //     this.$ready = true
        //     this.$s['Component.completed'].connections = {}
        // }
    }
    $uP(step = 0){
        let errors = []
        let errorsSignal = []
        let queueSignals = []

        for(let child of this.children){
            child.$uP(step + 1)
        }
        
        while(this.$uL.properties.length){
            let propName = this.$uL.properties.shift()
            try {
                for(let s of this.$p[propName].depends){
                    delete s[this.$p[propName].PID]
                }
                this.$p[propName].depends.clear()

                let val = this.$p[propName].func()
                if(this.$p[propName].val !== val){
                    this.$p[propName].val = val
                    if(queueSignals.indexOf(this.$p[propName].signal) < 0) queueSignals.push(this.$p[propName].signal)          
                }
            } catch (error) {
                errors.push({
                    obj: this,
                    propName: propName,
                })
                console.error(`${propName}`, this)
            }
            
            
            if(this.$uL.properties.length === 0){
                while(queueSignals.length){
                    let signal = queueSignals.shift()
                    signal()
                }
            }
            
        }
        

        
        

        while(this.$uL.aliases.length){
            let propName = this.$uL.aliases.shift()
            caller = this.$p[propName]
            let val = caller.func()
            caller = null
            
            if(this.$p[propName].val !== val){
                for(let s of this.$p[propName].depends){
                    delete s[this.$p[propName].PID]
                }
                this.$p[propName].depends.clear()

                this.$p[propName].val = val
                if(this.$p[propName].getter() !== val){
                    this.$p[propName].setter(val)
                }
                if(queueSignals.indexOf(this.$p[propName].signal) < 0) queueSignals.push(this.$p[propName].signal)     
            }

            if(this.$uL.aliases.length === 0){
                while(queueSignals.length){
                    let signal = queueSignals.shift()
                    signal()
                }
            }
            
        }



        for(let error of errors){
            let val = error.obj.$p[error.propName].func()
            if(error.obj.$p[error.propName].val !== val){

                for(let s of error.obj.$p[error.propName].depends){
                    delete s[error.obj.$p[error.propName].PID]
                }
                this.$p[propName].depends.clear()

                error.obj.$p[error.propName].val = val
                if(queueSignals.indexOf(error.obj.$p[error.propName].signal) < 0) queueSignals.push(error.obj.$p[error.propName].signal)   

            }
        }

        while(queueSignals.length){
            // try {
                let signal = queueSignals.shift()
                signal()
            // } catch (error) {
                
            // }
            
        }
        // for(let errorSignal of errorsSignal){
        //     try {
        //         errorSignal.obj.$p[errorSignal.propName].signal()
        //     } catch {
        //         console.log(`Signal for ${errorSignal.propName} property was called error`)
        //     }
            
        // }

        if(step === 0){
            this.$tryComplete()
        }
        // Core.queueCompleted.push(this)
        
        // if(updated){
        //     this.$s['Component.completed']()// = Signal()
        // } else {
            
        // }
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

        let signal = this.$cS(`${name}Changed`)
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
        
        // this.$s[`${name}Changed`] = signal
        
        Object.defineProperty(this, name, {
            get: ()=>{ 
                if(caller && caller !== this.$p[name]){
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
        let signal = this.$cS(`${name}Changed`)
        signal.debug = `${this.UID}-${name}`

        // this.$s[`${name}Changed`] = signal

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
                    if(caller && caller !== this.$p[`${name}.${name2}`]){
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
            if(this.$p[name].type === 'alias'){
                this.$p[name].func = ()=>{
                    caller = this.$p[name]
                    let res = func()
                    caller = null
                    return res
                }
                this.$uL.aliases.push(name)
            } else {
                this.$p[name].func = ()=>{
                    caller = this.$p[name]
                    let res = func()
                    caller = null
                    return res
                }
                this.$uL.properties.push(name)
            }
            
        } else {
            this.$cP(name, func)
        }
        
    }

    $cA(name, getter, setter){
        let signal = this.$cS(`${name}Changed`)

        this.$p[name] = {
            'val': '',
            'signal': signal,
            'type': 'alias',
            'setter': setter,
            'getter': getter,
            'depends': new Set(),
            'func': ()=>{
                caller = this.$p[name]
                let res = getter()
                caller = null
                return res
            },
            'PID': PID++
        }
        //this.$s[`${name}Changed`] = signal
        
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

    }

    $cS(name, ...args){
        let signal = Signal(args)
        
        this.$s[name] = signal
        this[name] = signal
        return signal
    }

    $updateGeometry(){
        
    }
    
    $domCreate(){
        
    }
    $updateChildrenRect(){
        
    }
    $updateRect(){
        
    }

    $setDom(dom){
        this.dom = dom
    }

    $destroy(){
        if(this.$repeater) this.$repeater.itemRemoved(this.index, this)

        for(let child of this.children){
            child.parent = null
            child.$destroy()
        }
        
        this.$s['Component.destruction']()
        delete UIDList[this.UID]

        this.$uL.properties = []
        this.$uL.aliases = []

        if(this.ID.size){
            IDManager.remove(this)
        }
        
        this.children = []
        if(this.parent){
            let indx = this.parent.children.indexOf(this)
            if(indx >= 0) {
                this.parent.children.splice(indx, 1)
                this.parent.$childChanged()
            }

        }
        for(let propName in this.$p){
            
            for(let s of this.$p[propName].depends){
                // delete s[this.$p[propName]]
                delete s.connections[this.$p[propName].PID]
            }
            this.$p[propName].depends.clear()
            this.$p[propName].signal.connections = {}      
        }
        // if(this.$timer) clearTimeout(this.$timer)
        setTimeout(()=>{
            
            for(let sigName in this.$s){
                this.$s[sigName].connections = {}
                for(let key in this.$s[sigName].repeats){
                    clearTimeout(this.$s[sigName].repeats[key])
                }
            }
            for(let key in this){
                delete this[key]
            }
        }, 100)

        
    
        
        
    }

    createComponent(_componentPath, _parent){
        return Core.cC(_componentPath, {parent: this, index: this.index})
    }
    
}


QML.QtObject = QtObject
