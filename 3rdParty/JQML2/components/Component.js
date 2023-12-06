const { QQmlComponent } = require('./QQmlComponent')

class Component extends QQmlComponent {
    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$exCtx = exCtx
    }
}

module.exports.Component = Component