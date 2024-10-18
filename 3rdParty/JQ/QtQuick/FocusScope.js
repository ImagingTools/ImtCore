const Item = require("./Item")

class FocusScope extends Item {
    onFocusChanged(){
        super.onFocusChanged()

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