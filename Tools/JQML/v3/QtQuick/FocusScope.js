const Item = require("./Item")

class FocusScope extends Item {
    SLOT_focusChanged(oldValue, newValue){
        super.SLOT_focusChanged(oldValue, newValue)

        if(!newValue){
            for(let child of this.children){
                child.activeFocus = false
            }
        }
    }
}



module.exports = FocusScope