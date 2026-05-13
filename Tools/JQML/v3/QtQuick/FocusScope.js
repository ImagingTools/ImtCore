const Item = require("./Item")

class FocusScope extends Item {
    SLOT_focusChanged(oldValue, newValue){
        super.SLOT_focusChanged()

        if(this.focus){
            for(let child of this.children){
                if(child.focus) child.activeFocus = true
            }
        } else {
            for(let child of this.children){
                child.activeFocus = false
            }
        }
    }
}



module.exports = FocusScope