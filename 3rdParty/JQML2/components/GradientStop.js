const { QtObject } = require('./QtObject')
const { QColor, QReal } = require('../utils/properties')

class GradientStop extends QtObject {
    static defaultProperties = {
        color: { type: QColor, value: 'black', changed: '$gradientUpdate' },
        position: { type: QReal, value: 0, changed: '$gradientUpdate' },
    }

    constructor(parent){
        super(parent)

        parent.getProperty('stops').get().push(this)
        parent.$gradientUpdate()
    }
    
    $gradientUpdate(){
        if(this.parent()){
            this.parent().$gradientUpdate()
        }
    }
    $destroy(){
        if(this.parent()){
            let index = this.parent().getProperty('stops').get().indexOf(this)
            if(index >= 0){
                this.parent().getProperty('stops').get().splice(index, 1)
                this.parent().gradientUpdate()
            }
        }
        
        super.$destroy()
    }
    
}

module.exports.GradientStop = GradientStop