const Animation = require("./Animation")
const Var = require("../QtQml/Var")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

class PropertyAction extends Animation {
    static meta = Object.assign({}, Animation.meta, {
        target: { type: Var, value: undefined },
        targets: { type: Var, value: undefined },
        property: { type: String, value: '' },
        properties: { type: String, value: '' },
        value: { type: Var, value: undefined },

        targetChanged: { type: Signal, args: [] },
        targetsChanged: { type: Signal, args: [] },
        propertyChanged: { type: Signal, args: [] },
        propertiesChanged: { type: Signal, args: [] },
        valueChanged: { type: Signal, args: [] },
    })

    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.target = parent
        return obj
    }

    __tick(){
        if(!this.running) return

        const targets = []
        if(this.target) targets.push(this.target)
        if(Array.isArray(this.targets)) for(const x of this.targets) if(x) targets.push(x)

        const propsStr = this.properties || this.property
        const props = propsStr ? propsStr.split(',').map(s => s.trim()).filter(Boolean) : []

        for(const tgt of targets){
            for(const p of props){
                try { tgt[p] = this.value } catch(e) {}
            }
        }

        this.running = false
        this.stopped()
        this.finished()
    }
}

module.exports = PropertyAction
