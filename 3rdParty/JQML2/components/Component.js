const { QQmlComponent } = require('./QQmlComponent')

class Component extends QQmlComponent {
    constructor(parent, exCtx){
        super(parent)
        this.$exCtx = exCtx
    }
}

module.exports.Component = Component