const { QtObject } = require('./QtObject')
const { QVar, QReal } = require('../utils/properties')

class Gradient extends QtObject {
    static Vertical = 0
    static Horizontal = 1

    static defaultProperties = {
        stops: { type: QVar, value: [], changed: '$gradientUpdate' },
        orientation: { type: QReal, value:  Gradient.Vertical, changed: '$gradientUpdate' },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
    }

    $gradientUpdate(){
        if(this.parent){
            this.parent.$gradientChanged()
        }
    }
}

module.exports.Gradient = Gradient