const { QtObject } = require('./QtObject')
const { QString } = require('../utils/properties')

class Href extends QtObject {
    static defaultProperties = {
        url: { type: QString, value: '' },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)

        this.$interval = setInterval(()=>{
            this.getProperty('url').reset(location.href)
        }, 50)
    }

    $destroy(){
        clearInterval(this.$interval)
        super.$destroy()
    }
    
}

module.exports.Href = Href