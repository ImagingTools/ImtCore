const { Item } = require('./Item')
const { QReal, QBool, QString } = require('../utils/properties')

class FocusScope extends Item {


    // $focusChanged(){
    //     if(this.getPropertyValue('focus')){
    //         if(this.getPropertyValue('context').$focusedElement){
    //             this.getPropertyValue('context').$focusedElement.getProperty('focus').reset(false)
    //         }
    //         this.getPropertyValue('context').$focusedElement = this
    //     }
        
    // }
 
}

module.exports.FocusScope = FocusScope