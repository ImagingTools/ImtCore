const { QtObject } = require('./QtObject')
const { QString } = require('../utils/properties')

class FileIO extends QtObject {

    static defaultProperties = {
        source: { type: QString, value: '', changed: '$sourceChanged' },
    }

    static defaultSignals = {

    }

    constructor(parent,exCtx,exModel) {
        super(parent,exCtx,exModel)

    }

    read(){

    }

    write(data){

    }

    setSource(source){
        this.getProperty('source').reset(source)
    }

    $sourceChanged(){

    }
}

module.exports.FileIO = FileIO