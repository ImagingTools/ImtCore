const { Item } = require('./Item')
const { QReal, QBool, QString } = require('../utils/properties')

class FocusScope extends Item {
    $setFocusedElement(element){
        if(mainRoot.$focusedElement && mainRoot.$focusedElement !== this && mainRoot.$focusedElement.UID) {
            mainRoot.$focusedElement.focus = false
            mainRoot.$focusedElement.activeFocus = false
            delete mainRoot.$focusedElement
        }

        this.focus = true
        
        super.$setFocusedElement(element)
    }

    $setActiveFocusedElement(element){
        if(mainRoot.$focusedElement && mainRoot.$focusedElement !== this && mainRoot.$focusedElement.UID) {
            mainRoot.$focusedElement.focus = false
            mainRoot.$focusedElement.activeFocus = false
            delete mainRoot.$focusedElement
        }

        this.focus = true
        this.activeFocus = true
        
        super.$setActiveFocusedElement(element)
    }
 
    $focusChanged(){
    //     if(this.focus){
    //         mainRoot.$setFocusedElement(this)

    //         if(this.$focusedElement && this.$focusedElement.UID) {
    //             this.$focusedElement.activeFocus = this.activeFocus
    //         }
    //     }
    }

    $activeFocusChanged(){
    //     if(this.activeFocus){
    //         mainRoot.$setFocusedElement(this)

    //         if(this.$focusedElement && this.$focusedElement.UID) {
    //             this.$focusedElement.focus = true
    //             this.$focusedElement.activeFocus = true
    //         }
    //     }
    }
 
}

module.exports.FocusScope = FocusScope