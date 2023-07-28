import {QtObject} from './QtObject'

export class Binding extends QtObject {
    static RestoreNone = 0
    static RestoreBinding = 1
    static RestoreValue = 2
    static RestoreBindingOrValue = 3

    constructor(args){
        super(args)

        this.$cP('delayed', false).connect(this.$update.bind(this))
        this.$cP('property', '').connect(this.$update.bind(this))
        this.$cP('restoreMode', Binding.RestoreBindingOrValue).connect(this.$update.bind(this))
        this.$cP('target', undefined).connect(this.$update.bind(this))
        this.$cP('value', undefined)//.connect(this.$update.bind(this))
        this.$cP('when', true).connect(this.$update.bind(this))

        this.$updated = false
        this.$prop = null
    }

    $update(){
        if(this.target && this.property && this.$updated){
            if(this.$prop) {
                delete this.$p.value.getSignal().connections[this.$prop.PID]
                this.$prop.depends.delete(this.$p.value.getSignal())
            }

            let path = this.property.split('.')
            let prop = this.target

            while(path.length > 2){
                let propName = path.shift()
                prop = prop[propName]
                // if(path.length){
                //     prop = prop.$p[propName].val
                // } else {
                //     prop = prop.$p[propName]
                // }
            }

            if(path.length === 2){
                if(prop.$p[`${path[0]}.${path[1]}`]){
                    prop = prop.$p[`${path[0]}.${path[1]}`]
                } else {
                    prop = prop[path[0]].$p[path[1]]
                }
            } else if(path.length === 1){
                prop = prop.$p[path[0]]
            }
            // caller = prop
            this.$prop = prop
            prop.depends.add(this.$p.value.getSignal())
            this.$p.value.getSignal().connectWithName(prop.PID, ()=>{
                if(this.$p.value.val !== prop.val){
                    prop.val = this.$p.value.val
                    if(prop.type === 'alias') prop.setter(prop.val)
                    prop.getSignal()()
                }
            })
            if(this.$p.value.val !== prop.val){
                prop.val = this.$p.value.val
                if(prop.type === 'alias') prop.setter(prop.val)
                prop.getSignal()()
            }
            // caller = null
        }
    }
}

QML.Binding = Binding