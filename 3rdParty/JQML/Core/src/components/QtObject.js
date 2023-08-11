import {Signal} from '../utils/Signal'

export class QtObject {
    $p = {}
    $s = {}
    $ready = false
    $completed = false
    $uL = {
        properties: [],
        aliases: [],
    }
    // context = {}
    ID = new Set()
    $availableGeometry = []
    // LVL = new Set()

    constructor(args) {
        if(args.$qmlClassName) args = {parent: args}
        if(args.parent && args.parent.$flickable && args.parent.contentItem) args.parent = args.parent.contentItem
        this.$PI = new Proxy(this, Core.proxyHandlerID)
        this.$P0 = new Proxy(this, Core.proxyHandler0)
        this.$qmlClassName = this.constructor.name
        // this._context = context
        this.context = Core.context
        this.UID = UID++
        UIDList[this.UID] = this
        this.children = []
        this.$treeChilds = []
        this.$treeParent = null
        this.$treeParent2 = null

        if(this.$qmlClassName !== 'ListElement') this.$cP('jqmlDebug', false)
        
        
        if('repeater' in args){
            this.$repeater = args.repeater
            delete args['repeater']
        }
        if('parent' in args && args.parent !== null){
            if(this.$qmlClassName !== 'ListElement'){
                this.$cP('index', args.parent.index)
                this.index = args.parent.index
            }
            

            args.parent.$treeChilds.push(this)
            
            if('pos' in args){
                args.parent.children.splice(args.pos, 0, this)
            } else {
                args.parent.children.push(this)
            }
            args.parent.$childChanged(this)
            this.parent = args.parent
            this.$treeParent = args.parent 
        }
        if('index' in args && this.$qmlClassName !== 'ListElement'){
            this.$cP('index', args.index)
            this.index = args.index
        }

        this.$cS('Component.completed')
        this.$cS('Component.destruction')

        // ReadyList.push(this)
        // console.log(this)
        Object.defineProperty(this, 'model', {
            get: ()=>{
                let handler = {
                    // parent: this,
                    has(){
                        return true
                    },
                    get(target, name){
                        return target[name]
                    },
                    set(target, name, value){
                        let oldValue = target[name]
                        if(oldValue !== value){
                            target[name] = value
                            // console.log('DEBUG::handler-set', this.parent, name, value)
                            this.model.$modelChanged()
                            this.model.$dataChanged()
                        }
                        
                        return true
                    },
                }
                if(this.$repeater){
                    if(typeof this.$repeater.$p.model.val === 'number'){     
                        return new Proxy({
                            'index': this.index
                        }, handler)
                    } else if (typeof this.$repeater.$p.model.val === 'object'){
                        if(Array.isArray(this.$repeater.$p.model.val)){
                            let target = {}
                            if(this.$repeater.$p.model.val[this.index]){
                                target = this.$repeater.$p.model.val[this.index]
                                if(target.$qmlClassName === 'ListElement') return target
                                handler.model = this.$repeater.$p.model.val
                            }
                            return new Proxy(target, handler)
                        } else {
                            let target = {}
                            if(this.$repeater.$p.model.val.$p.data.val[this.index]){
                                target = this.$repeater.$p.model.val.$p.data.val[this.index]
                                if(target.$qmlClassName === 'ListElement') return target
                                handler.model = this.$repeater.$p.model.val
                            }
                            return new Proxy(target, handler)
                        }
                    }
                }
                let parent = this.parent
                while(parent){
                    
                    if(parent.$repeater){
                        if(typeof parent.$repeater.$p.model.val === 'number'){     
                            return new Proxy({
                                'index': this.index
                            }, handler)
                        } else if (typeof parent.$repeater.$p.model.val === 'object'){
                            if(Array.isArray(parent.$repeater.$p.model.val)){
                                let target = {}
                                if(parent.$repeater.$p.model.val[this.index]){
                                    target = parent.$repeater.$p.model.val[this.index]
                                    if(target.$qmlClassName === 'ListElement') return target
                                    handler.model = parent.$repeater.$p.model.val
                                }
                                return new Proxy(target, handler)
                            } else {
                                let target = {}
                                if(parent.$repeater.$p.model.val.$p.data.val[this.index]){
                                    target = parent.$repeater.$p.model.val.$p.data.val[this.index]
                                    if(target.$qmlClassName === 'ListElement') return target
                                    handler.model = parent.$repeater.$p.model.val
                                }
                                return new Proxy(target, handler)
                            }
                            
                        }
                    }
                    if(parent.$mapView){
                        if(typeof parent.$mapView.$p.model.val === 'number'){     
                            return new Proxy({
                                'index': this.index
                            }, handler)
                        } else if (typeof parent.$mapView.$p.model.val === 'object'){
                            if(Array.isArray(parent.$mapView.$p.model.val)){
                                let target = {}
                                if(parent.$mapView.$p.model.val[this.index]){
                                    target = parent.$mapView.$p.model.val[this.index]
                                    if(target.$qmlClassName === 'ListElement') return target
                                    handler.model = parent.$mapView.$p.model.val
                                }
                                return new Proxy(target, handler)
                            } else {
                                let target = {}
                                if(parent.$mapView.$p.model.val.$p.data.val[this.index]){
                                    target = parent.$mapView.$p.model.val.$p.data.val[this.index]
                                    if(target.$qmlClassName === 'ListElement') return target
                                    handler.model = parent.$mapView.$p.model.val
                                }
                                return new Proxy(target, handler)
                            }
                            
                        }
                    }
                    if(parent.$useModel) {
                        if(typeof parent.$p.model.val === 'number'){     
                            return new Proxy({
                                'index': this.index
                            }, handler)
                        } else if (typeof parent.$p.model.val === 'object'){
                            if(Array.isArray(parent.$p.model.val)){
                                let target = {}
                                if(parent.$p.model.val[this.index]){
                                    target = parent.$p.model.val[this.index]
                                    if(target.$qmlClassName === 'ListElement') return target
                                    handler.model = parent.$p.model.val
                                }
                                return new Proxy(target, handler)
                            } else {
                                let target = {}
                                if(parent.$p.model.val.$p.data.val[this.index]){
                                    target = parent.$p.model.val.$p.data.val[this.index]
                                    if(target.$qmlClassName === 'ListElement') return target
                                    handler.model = parent.$p.model.val
                                }
                                return new Proxy(target, handler)
                            }
                            
                        }
                        
                    }
                    parent = parent.parent
                }
            },
            configurable: true,
        })
    }
    $childChanged(child){
        
    }
    $childDomChanged(child){

    }
    $tryComplete(){
        if(!this.$completed){
            this.$completed = true
            this['Component.completed']()
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
        if(this.UID === null || this.UID === undefined) return

        let errors = []
        let errorsSignal = []
        let queueSignals = []
        let bindings = []
        
        while(this.$uL.properties.length){
            let propName = this.$uL.properties.shift()
            try {
                for(let s of this.$p[propName].depends){
                    delete s[this.$p[propName].PID]
                }
                this.$p[propName].depends.clear()

                let val = this.$p[propName].func()
               
                if(this.$p[propName].val !== val){
                    if(this.jqmlDebug){
                        console.info(`JQML::updating property ${propName}. old = `, this.$p[propName].val, `new = `, val)
                    }
                    this.$p[propName].val = val
                    if(queueSignals.indexOf(this.$p[propName].getSignal()) < 0) queueSignals.push(this.$p[propName].getSignal())          
                }
            } catch (error) {
                errors.push({
                    obj: this,
                    propName: propName,
                })
                console.error(`${this.UID}::skip::property::${propName}`)
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
            this.$p[propName].getter()
            let val = caller.func()
            caller = null
            
            if(this.$p[propName].val !== val){
                for(let s of this.$p[propName].depends){
                    delete s[this.$p[propName].PID]
                }
                this.$p[propName].depends.clear()

                this.$p[propName].val = val
                if(this.$p[propName].getter() !== val){
                    if(this.jqmlDebug){
                        console.info(`JQML::updating aliase ${propName}. old = `, this.$p[propName].val, `new = `, val)
                    }
                    this.$p[propName].setter(val)
                }
                if(queueSignals.indexOf(this.$p[propName].getSignal()) < 0) queueSignals.push(this.$p[propName].getSignal())     
            }

            if(this.$uL.aliases.length === 0){
                while(queueSignals.length){
                    let signal = queueSignals.shift()
                    signal()
                }
            }
            
        }
        
        for(let i = this.children.length-1; i >= 0; i--){
            if(this.children[i].$qmlClassName === 'Binding') bindings.push(this.children[i])
            this.children[i].$uP(step + 1)
        }
        


        for(let error of errors){
            try {
                let val = error.obj.$p[error.propName].func()
                if(error.obj.$p[error.propName].val !== val){

                    for(let s of error.obj.$p[error.propName].depends){
                        delete s[error.obj.$p[error.propName].PID]
                    }
                    this.$p[propName].depends.clear()

                    error.obj.$p[error.propName].val = val
                    if(queueSignals.indexOf(error.obj.$p[error.propName].getSignal()) < 0) queueSignals.push(error.obj.$p[error.propName].getSignal())   

                }
            } catch {
                console.error(`${this.UID}::skip::property::${error.propName}`)
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

        while(bindings.length){
            let binding = bindings.shift()
            if(!binding.$updated){
                binding.$updated = true
                binding.$update()
            }
            
        }

        if(step === 0){
            this.$tryComplete()
        }
        // for(let loader of Core.queueLoader){
        //     if(loader.UID) loader.$uP()
        // }
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
    

    $cP(name, val, changed){
        if(name in this.$p){
            return //this.$s[`${name}Changed`]
        }

        this.$cS(`${name}Changed`)

        if(typeof val === 'function'){
            this.$p[name] = {
                'val': undefined,
                'changed': true,
                // 'freeze': false,
                // 'signal': this.$cS(`${name}Changed`),
                'depends': new Set(),
                'func': ()=>{
                    caller = this.$p[name]
                    let res = val()
                    caller = null
                    if(typeof res === 'number'){
                        if(!isNaN(res)){
                            return res
                        } else {
                            if(res === Infinity){
                                return -2147483648
                            } else {
                                return 0
                            }
                        }
                    } else {
                        return res
                    }
                },
                'PID': PID++
            }

            this.$uL.properties.push(name)
        } else {
            if(typeof val === 'number'){
                if(isNaN(val)){
                    val = 0
                }
                if(val === Infinity){
                    val = -2147483648
                }
    
            }
            this.$p[name] = {
                'val': val,
                'changed': true,
                // 'freeze': false,
                // 'signal': signal,
                // 'depends': new Set(),
                'PID': PID++
            }
        }

        // if(changed) this.$p[name].changed = changed.bind(this)
        
        this.$p[name].getSignal = ()=>{
            if(!this.$s[`${name}Changed`]) {
                this.$s[`${name}Changed`] = Signal()
            }
            if(changed && this.$p[name].changed){
                this.$s[`${name}Changed`].connect(changed.bind(this))
                delete this.$p[name].changed
            }
            return this.$s[`${name}Changed`]
        }
        
        // this.$s[`${name}Changed`] = signal
        
        Object.defineProperty(this, name, {
            get: ()=>{ 
                if(this.UID === null || this.UID === undefined) return

                if(caller && caller !== this.$p[name]){
                    let _caller = caller
                    if(_caller.type === 'alias'){
                        this.$p[name].getSignal().connectWithName(_caller.PID, ()=>{
                            let val1 = _caller.func ? _caller.func() : _caller.val
                            let val2 = _caller.getter()
                            if(val1 !== _caller.val){
                                _caller.val = val1
                                _caller.setter(_caller.val)
                                
                                _caller.getSignal()()
                            } else if (val2 !== _caller.val) {
                                _caller.val = val2
                                
                                _caller.getSignal()()
                            }
                        })
                    } else {
                        if(!_caller.depends) _caller.depends = new Set()
                        _caller.depends.add(this.$p[name].getSignal())
                        this.$p[name].getSignal().connectWithName(_caller.PID, ()=>{
                            let val = _caller.func()
                            if(_caller.val !== val){
                                _caller.val = val
                                
                                _caller.getSignal()()
                            }
                        })
                    }
                }
                
                return this.$p[name].val
            },
            set: (newVal)=>{
                if(this.UID === null || this.UID === undefined) return

                while(this.$uL.properties.indexOf(name) >= 0){
                    this.$uL.properties.splice(this.$uL.properties.indexOf(name), 1)
                }
                while(this.$uL.aliases.indexOf(name) >= 0){
                    this.$uL.aliases.splice(this.$uL.aliases.indexOf(name), 1)
                }

                let isBinding = false
                if(typeof newVal === 'function' && newVal.type === 'Binding'){

                    if(this.$p[name].depends){
                        for(let s of this.$p[name].depends){
                            s.disconnectWithName(this.$p[name].PID)
                        }
                        this.$p[name].depends.clear()
                        delete this.$p[name].depends
                    }
                    
                    
                    caller = this.$p[name]
                    this.$p[name].func = newVal
                    newVal = this.$p[name].func()
                    caller = null
                    isBinding = true
                }

                if(this.$p[name].val !== newVal){
                    if(this.jqmlDebug){
                        console.info(`JQML::set property ${name}. old = `, this.$p[name].val, `new = `, newVal)
                    }
                    if(this.$p[name].freeze === true){
                        
                        this.$p[name].getSignal()()
                        this.$p[name].getSignal()()
                    } else {
                        if(!isBinding && this.$p[name].depends){
                            for(let s of this.$p[name].depends){
                                s.disconnectWithName(this.$p[name].PID)
                            }
                            this.$p[name].depends.clear()
                            delete this.$p[name].depends
                        }
                      
                        if(name === 'visible' && this.parent){
                            this.$sP('visible', ()=>{return this.parent.visible && newVal})
                            let val = this.$p[name].func()
                            if(this.$p[name].val !== val){
                                this.$p[name].val = val
                                
                                this.$p[name].getSignal()()
                            }
                        } else {
                            this.$p[name].func = null
                            this.$p[name].val = newVal
                            
                            this.$p[name].getSignal()()
                        }
                        
                    }
                    
                }              
            },
        })
        
    }

    $cPC(name, props, changed){
        if(name in this.$p){
            return
        }
   
        this.$cS(`${name}Changed`)

        this[name] = {

        }

        

        for(let name2 in props){
            
            this.$p[`${name}.${name2}`] = {
                'val': props[name2],
                'changed': true,
                // 'signal': signal,
                // 'depends': new Set(),
                'PID': PID++,
            }

            this.$p[`${name}.${name2}`].getSignal = ()=>{
                if(!this.$s[`${name}Changed`]) {
                    this.$s[`${name}Changed`] = Signal()
                }
                if(changed && this.$p[`${name}.${name2}`].changed){
                    this.$s[`${name}Changed`].connect(changed.bind(this))
                    delete this.$p[`${name}.${name2}`].changed
                }
                return this.$s[`${name}Changed`]
            }

            Object.defineProperty(this[name], name2, {
                get: ()=>{ 
                    if(this.UID === null || this.UID === undefined) return

                    if(caller && caller !== this.$p[`${name}.${name2}`]){
                        let _caller = caller
                        if(_caller.type === 'alias'){
                            this.$p[`${name}.${name2}`].getSignal().connectWithName(_caller.PID, ()=>{
                                let val1 = _caller.func ? _caller.func() : _caller.val
                                let val2 = _caller.getter()
                                if(val1 !== _caller.val){
                                    _caller.val = val1
                                    _caller.setter(_caller.val)
                                    
                                    _caller.getSignal()()
                                } else if (val2 !== _caller.val) {
                                    _caller.val = val2
                                    
                                    _caller.getSignal()()
                                }
                            })
                        } else {
                            if(!_caller.depends) _caller.depends = new Set()
                            _caller.depends.add(this.$p[`${name}.${name2}`].getSignal())
                            this.$p[`${name}.${name2}`].getSignal().connectWithName(_caller.PID, ()=>{
                                let val = _caller.func()
                                if(_caller.val !== val){
                                    _caller.val = val
                                    
                                    _caller.getSignal()()
                                }
                            })
                        }
                    }

                    return this.$p[`${name}.${name2}`].val
                },
                set: (newVal)=>{
                    if(this.UID === null || this.UID === undefined) return

                    while(this.$uL.properties.indexOf(`${name}.${name2}`) >= 0){
                        this.$uL.properties.splice(this.$uL.properties.indexOf(`${name}.${name2}`), 1)
                    }
                    while(this.$uL.aliases.indexOf(`${name}.${name2}`) >= 0){
                        this.$uL.aliases.splice(this.$uL.aliases.indexOf(`${name}.${name2}`), 1)
                    }

                    let isBinding = false
                    if(typeof newVal === 'function' && newVal.type === 'Binding'){
                        if(this.$p[`${name}.${name2}`].depends){
                            for(let s of this.$p[`${name}.${name2}`].depends){
                                s.disconnectWithName(this.$p[`${name}.${name2}`].PID)
                            }
                            this.$p[`${name}.${name2}`].depends.clear()
                            delete this.$p[`${name}.${name2}`].depends
                        }

                        caller = this.$p[`${name}.${name2}`]
                        this.$p[`${name}.${name2}`].func = newVal
                        newVal = this.$p[`${name}.${name2}`].func()
                        caller = null
                        isBinding = true
                    }

                    if(this.$p[`${name}.${name2}`].val !== newVal){
                        if(this.jqmlDebug){
                            console.info(`JQML::set property ${name}.${name2}. old = `, this.$p[`${name}.${name2}`].val, `new = `, newVal)
                        }
                        if(!isBinding && this.$p[`${name}.${name2}`].depends){
                            for(let s of this.$p[`${name}.${name2}`].depends){
                                s.disconnectWithName(this.$p[`${name}.${name2}`].PID)
                            }
                            this.$p[`${name}.${name2}`].depends.clear()
                            delete this.$p[`${name}.${name2}`].depends
                        }
                        this.$p[`${name}.${name2}`].func = null
                        this.$p[`${name}.${name2}`].val = newVal
    
                   
                        this.$p[`${name}.${name2}`].getSignal()()
                    }
                },
            })
        }
        // return signal
    }

    $sP(name, func){
        if(this.UID === null || this.UID === undefined) return
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
            if(!this.$p[name].depends){
                this.$p[name].depends = new Set()
            }
            if(this.$p[name].type === 'alias'){
                this.$p[name].func = ()=>{
                    if(this.UID === null || this.UID === undefined) return
                    caller = this.$p[name]
                    let res = func()
                    caller = null
                    if(typeof res === 'number'){
                        if(!isNaN(res)){
                            return res
                        } else {
                            if(res === Infinity){
                                return -2147483648
                            } else {
                                return 0
                            }
                        }
                    } else {
                        return res
                    }
                }
                this.$uL.aliases.push(name)
            } else {
                this.$p[name].func = ()=>{
                    if(this.UID === null || this.UID === undefined) return
                    caller = this.$p[name]
                    let res = func()
                    caller = null
                    if(typeof res === 'number'){
                        if(!isNaN(res)){
                            return res
                        } else {
                            if(res === Infinity){
                                return -2147483648
                            } else {
                                return 0
                            }
                        }
                    } else {
                        return res
                    }
                }
                this.$uL.properties.push(name)
            }
            
        } else {
            this.$cP(name, func)
        }
        
    }

    $cA(name, getter, setter){
        // let signal = this.$cS(`${name}Changed`)

        // this.$p[name] = {
        //     'val': '',
        //     'signal': signal,
        //     'type': 'alias',
        //     'setter': setter,
        //     'getter': getter,
        //     'depends': new Set(),
        //     'func': ()=>{
        //         caller = this.$p[name]
        //         let res = getter()
        //         caller = null
        //         return res
        //     },
        //     'PID': PID++
        // }
        // //this.$s[`${name}Changed`] = signal
        
        // Object.defineProperty(this, name, {
        //     get: ()=>{ 
        //         if(caller){
        //             let _caller = caller
        //             if(_caller.type === 'alias'){
        //                 signal.connections[_caller.PID] = ()=>{
        //                     let val = _caller.func()
        //                     if(val !== _caller.getter()){
        //                         _caller.setter(val)
        //                     } else {
        //                         _caller.getSignal()()
        //                     }
                            
        //                 }
        //             } else {
        //                 _caller.depends.add(signal)
        //                 signal.connections[_caller.PID] = ()=>{
        //                     let val = _caller.func()
        //                     if(_caller.val !== val){
        //                         _caller.val = val
        //                         _caller.getSignal()()
        //                     }
        //                 }
        //             }
                    
        //         }
                
        //         return this.$p[name].val
        //     },
        //     set: (newVal)=>{
        //         setter(newVal)          
        //     },
        // })

        // // PropertyManager.add(this.$p[name])
        // this.$uL.aliases.push(name)
        // return signal

        if(typeof val === 'number'){
            if(isNaN(val)){
                val = 0
            }
            if(val === Infinity){
                val = -2147483648
            }

        }

        this.$cS(`${name}Changed`)
        // let signal = this.$cS(`${name}Changed`)
        // signal.debug = `${this.UID}-${name}`

        this.$p[name] = {
            'val': undefined,
            // 'signal': signal,
            'depends': new Set(),
            'type': 'alias',
            'func': ()=>{
                caller = this.$p[name]
                let res = getter()
                caller = null
                if(typeof res === 'number'){
                    if(!isNaN(res)){
                        return res
                    } else {
                        if(res === Infinity){
                            return -2147483648
                        } else {
                            return 0
                        }
                    }
                } else {
                    return res
                }
            },
            'getter': getter,
            'setter': setter,
            'PID': PID++
        }

        this.$p[name].getSignal = ()=>{
            if(!this.$s[`${name}Changed`]) {
                this.$s[`${name}Changed`] = Signal()
            }
            return this.$s[`${name}Changed`]
        }

        Object.defineProperty(this, name, {
            get: ()=>{ 
                if(this.UID === null || this.UID === undefined) return

                if(caller && caller !== this.$p[name]){
                    let _caller = caller
                    if(_caller.type === 'alias'){
                        this.$p[name].getSignal().connectWithName(_caller.PID, ()=>{
                            let val1 = _caller.func ? _caller.func() : _caller.val
                            let val2 = _caller.getter()
                            if(val1 !== _caller.val){
                                _caller.val = val1
                                _caller.setter(_caller.val)
                                
                                _caller.getSignal()()
                            } else if (val2 !== _caller.val) {
                                _caller.val = val2
                                
                                _caller.getSignal()()
                            }
                        })
                    } else {
                        if(!_caller.depends) _caller.depends = new Set()
                        _caller.depends.add(this.$p[name].getSignal())
                        this.$p[name].getSignal().connectWithName(_caller.PID, ()=>{
                            let val = _caller.func()
                            if(_caller.val !== val){
                                _caller.val = val
                                
                                _caller.getSignal()()
                            }
                        })
                    }
                }
                
                return this.$p[name].val
            },
            set: (newVal)=>{
                if(this.UID === null || this.UID === undefined) return

                while(this.$uL.properties.indexOf(name) >= 0){
                    this.$uL.properties.splice(this.$uL.properties.indexOf(name), 1)
                }
                while(this.$uL.aliases.indexOf(name) >= 0){
                    this.$uL.aliases.splice(this.$uL.aliases.indexOf(name), 1)
                }

                let isBinding = false
                if(typeof newVal === 'function' && newVal.type === 'Binding'){
                    if(this.$p[name].depends){
                        for(let s of this.$p[name].depends){
                            s.disconnectWithName(this.$p[name].PID)
                        }
                        this.$p[name].depends.clear()
                        delete this.$p[name].depends
                    }

                    caller = this.$p[name]
                    this.$p[name].func = newVal
                    newVal = this.$p[name].func()
                    caller = null
                    isBinding = true
                }

                if(this.$p[name].val !== newVal){
                    if(!isBinding && this.$p[name].depends){
                        for(let s of this.$p[name].depends){
                            s.disconnectWithName(this.$p[name].PID)
                        }
                        this.$p[name].depends.clear()
                        delete this.$p[name].depends
                        this.$p[name].func = null
                    }
                    

                    this.$p[name].val = newVal
                    setter(newVal)
                    this.$p[name].getSignal()()
                }              
            },
        })
        this.$uL.aliases.push(name)
       
    }

    $cS(name, ...args){
        // if(name in this) return
        Object.defineProperty(this, name, {
            get: ()=>{
                if(!this.$s[name]) this.$s[name] = Signal(args)
                return this.$s[name]
            }
        })
    }

    $updateGeometry(){
        
    }
    
    $domCreate(tag = 'div'){
        
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
        
        this['Component.destruction']()
        delete UIDList[this.UID]

        this.$uL.properties = []
        this.$uL.aliases = []

        if(this.ID.size){
            IDManager.remove(this)
        }
        
        this.children = []
        if(this.parent){
            let indx = this.parent.$availableGeometry.indexOf(this)
            if(indx >= 0) {
                this.parent.$availableGeometry.splice(indx, 1)
            }

            indx = this.parent.children.indexOf(this)
            if(indx >= 0) {
                this.parent.children.splice(indx, 1)
                this.parent.$childChanged(this)
            }

            
        }
        for(let propName in this.$p){
            if(this.$p[propName].depends){
                for(let s of this.$p[propName].depends){
                    // delete s[this.$p[propName]]
                    s.disconnectWithName(this.$p[propName].PID)
                }
                this.$p[propName].depends.clear()
            }
            
            delete this.$p[propName].getSignal().connections
        }
        // if(this.$timer) clearTimeout(this.$timer)
        setTimeout(()=>{
            
            for(let sigName in this.$s){
                delete this.$s[sigName].connections
                // for(let key in this.$s[sigName].repeats){
                //     clearTimeout(this.$s[sigName].repeats[key])
                // }
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
