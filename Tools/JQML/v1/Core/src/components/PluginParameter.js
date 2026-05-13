import {QtObject} from './QtObject'

export class PluginParameter extends QtObject  {
    constructor(args) {
        super(args)
        this.$cP('name', '', this.$parameterChanged)
        this.$cP('value', '', this.$parameterChanged)

        this.parent.parameters.push(this)
        // this.parent.$p.parameters.getSignal()()
    }

    $parameterChanged(){
        this.parent.$p.parameters.getSignal()()
    }

    $destroy(){
        let index = this.parent.parameters.indexOf(this)
        if(index >= 0) this.parent.parameters.splice(index, 1)
    }

}

QML.PluginParameter = PluginParameter