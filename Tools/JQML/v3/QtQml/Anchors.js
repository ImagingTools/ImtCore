const GroupProperty = require("./GroupProperty")
const AnchorLine = require("./AnchorLine")
const Real = require("./Real")
const Var = require("./Var")
const Signal = require("./Signal")
const QtFunctions = require("../Qt/functions")


class Anchors extends GroupProperty {
    static meta = {
        fill: { type: Var, value: undefined},
        centerIn: { type: Var, value: undefined},

        left: { type: Var, value: undefined},
        right: { type: Var, value: undefined},
        top: { type: Var, value: undefined},
        bottom: { type: Var, value: undefined},
        horizontalCenter: { type: Var, value: undefined},
        verticalCenter: { type: Var, value: undefined},

        leftMargin: { type: Real, value: 0},
        topMargin: { type: Real, value: 0},
        rightMargin: { type: Real, value: 0},
        bottomMargin: { type: Real, value: 0},
        margins: { type: Real, value: 0},
        horizontalCenterOffset: { type: Real, value: 0},
        verticalCenterOffset: { type: Real, value: 0},

        fillChanged: {type:Signal, args:[]},
        centerInChanged: {type:Signal, args:[]},


        leftMarginChanged: {type:Signal, args:[]},
        topMarginChanged: {type:Signal, args:[]},
        rightMarginChanged: {type:Signal, args:[]},
        bottomMarginChanged: {type:Signal, args:[]},

        leftChanged: {type:Signal, args:[]},
        topChanged: {type:Signal, args:[]},
        rightChanged: {type:Signal, args:[]},
        bottomChanged: {type:Signal, args:[]},
        horizontalCenterChanged: {type:Signal, args:[]},
        verticalCenterChanged: {type:Signal, args:[]},

        marginsChanged: {type:Signal, args:[]},
        horizontalCenterOffsetChanged: {type:Signal, args:[]},
        verticalCenterOffsetChanged: {type:Signal, args:[]}
    }

    SLOT_fillChanged(oldValue, newValue){
        let target = newValue
        let self = this.__parent.__proxy

        if(self.parent === target){
            self.AX = ()=>{return this.__proxy.leftMargin}
            self.AY = ()=>{return this.__proxy.topMargin}
            self.AWidth = ()=>{return target.AWidth - (this.__proxy.leftMargin + this.__proxy.rightMargin)}
            self.AHeight = ()=>{return target.AHeight - (this.__proxy.topMargin + this.__proxy.bottomMargin)}
        } else {
            self.AX = ()=>{return target.AX + this.__proxy.leftMargin}
            self.AY = ()=>{return target.AY + this.__proxy.topMargin}
            self.AWidth = ()=>{return target.AWidth - (this.__proxy.leftMargin + this.__proxy.rightMargin)}
            self.AHeight = ()=>{return target.AHeight - (this.__proxy.topMargin + this.__proxy.bottomMargin)}
        }
    }

    SLOT_centerInChanged(oldValue, newValue){
        let target = newValue
        let self = this.__parent.__proxy

        if(target){
            if(self.parent === target){
                self.AX = ()=>{return target.AWidth / 2 - self.AWidth / 2}
                self.AY = ()=>{return target.AHeight / 2 - self.AHeight / 2}
            } else {
                self.AX = ()=>{return target.AX + target.AWidth / 2 - self.AWidth / 2}
                self.AY = ()=>{return target.AY + target.AHeight / 2 - self.AHeight / 2}
            }
        }
        
    }

    SLOT_marginsChanged(oldValue, newValue){
        this.__proxy.leftMargin = this.margins
        this.__proxy.topMargin = this.margins
        this.__proxy.rightMargin = this.margins
        this.__proxy.bottomMargin = this.margins
    }

    SLOT_horizontalCenterChanged(oldValue, newValue){
        let target = newValue
        let self = this.__parent.__proxy

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent.__proxy

            if(self.parent === target){
                if(targetFloat === AnchorLine.Left){
                    self.AX = ()=>{return -self.AWidth / 2 + this.__proxy.horizontalCenterOffset}
                } else if(targetFloat === AnchorLine.Right){
                    self.AX = ()=>{return target.AWidth - self.AWidth / 2 + this.__proxy.horizontalCenterOffset}
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.AX = ()=>{return target.AWidth / 2 - self.AWidth / 2 + this.__proxy.horizontalCenterOffset}
                }
            } else {
                if(targetFloat === AnchorLine.Left){
                    self.AX = ()=>{return target.AX - self.AWidth / 2 + this.__proxy.horizontalCenterOffset}
                } else if(targetFloat === AnchorLine.Right){
                    self.AX = ()=>{return target.AX + target.AWidth - self.AWidth / 2 + this.__proxy.horizontalCenterOffset}
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.AX = ()=>{return target.AX + target.AWidth / 2 - self.AWidth / 2 + this.__proxy.horizontalCenterOffset}
                }
            }
        }
    }

    SLOT_verticalCenterChanged(oldValue, newValue){
        let target = newValue
        let self = this.__parent.__proxy

        if(self && target){
            let targetFloat = target.__float
            target = target.__parent.__proxy

            if(self.parent === target){
                if(targetFloat === AnchorLine.Top){
                    self.AY = ()=>{return -self.AHeight / 2 + this.__proxy.verticalCenterOffset}
                } else if(targetFloat === AnchorLine.Bottom){
                    self.AY = ()=>{return target.AHeight - self.AHeight / 2 + this.__proxy.verticalCenterOffset}
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.AY = ()=>{return target.AHeight / 2 - self.AHeight / 2 + this.__proxy.verticalCenterOffset}
                }
            } else {
                if(targetFloat === AnchorLine.Top){
                    self.AY = ()=>{return target.AY - self.AHeight / 2 + this.__proxy.verticalCenterOffset}
                } else if(targetFloat === AnchorLine.Bottom){
                    self.AY = ()=>{return target.AY + target.AHeight - self.AHeight / 2 + this.__proxy.verticalCenterOffset}
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.AY = ()=>{return target.AY + target.AHeight / 2 - self.AHeight / 2 + this.__proxy.verticalCenterOffset}
                }               
            }
        }
    }

    SLOT_leftChanged(oldValue, newValue){
        if(newValue && newValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_x === 2){
                        self.AX = self.AX
                        self.AWidth = self.AWidth
                    } else if(this.__for_x === 1){
                        self.AX = self.AX
                    }
                }
                this.left = null
            }
            destructionFunc.meta = {
                name: 'anchors_left__destruction',
                destruction: true,
                parent: newValue.__parent.__proxy
            }
            newValue.__parent.__proxy.__addLink()
            if(newValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = newValue.__parent.__proxy
                newValue.__parent.__proxy['Component.destruction'].connect(destructionFunc)
            }
        }
        if(oldValue && oldValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_x === 2){
                        self.AX = self.AX
                        self.AWidth = self.AWidth
                    } else if(this.__for_x === 1){
                        self.AX = self.AX
                    }
                }
                this.left = null
            }
            destructionFunc.meta = {
                name: 'anchors_left__destruction',
                destruction: true,
                parent: oldValue.__parent.__proxy
            }
            if(oldValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = oldValue.__parent.__proxy
                oldValue.__parent.__proxy['Component.destruction'].disconnect(destructionFunc)
            }
            oldValue.__parent.__proxy.__removeLink()
        }

        this.onLeftRightChanged()
    }

    SLOT_rightChanged(oldValue, newValue){
        if(newValue && newValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_x === 2){
                        self.AX = self.AX
                        self.AWidth = self.AWidth
                    } else if(this.__for_x === 1){
                        self.AX = self.AX
                    }
                }
                this.right = null
            }
            destructionFunc.meta = {
                name: 'anchors_right__destruction',
                destruction: true,
                parent: newValue.__parent.__proxy
            }
            newValue.__parent.__proxy.__addLink()
            if(newValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = newValue.__parent.__proxy
                newValue.__parent.__proxy['Component.destruction'].connect(destructionFunc)
            }
        }
        if(oldValue && oldValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_x === 2){
                        self.AX = self.AX
                        self.AWidth = self.AWidth
                    } else if(this.__for_x === 1){
                        self.AX = self.AX
                    }
                }
                this.right = null
            }
            destructionFunc.meta = {
                name: 'anchors_right__destruction',
                destruction: true,
                parent: oldValue.__parent.__proxy
            }
            if(oldValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = oldValue.__parent.__proxy
                oldValue.__parent.__proxy['Component.destruction'].disconnect(destructionFunc)
            }
            oldValue.__parent.__proxy.__removeLink()
        }

        this.onLeftRightChanged()
    }

    onLeftRightChanged(){
        let left = this.__proxy.left
        let right = this.__proxy.right
        let self = this.__parent.__proxy     

        this.__for_x = 0

        if(self && left && right){
            this.__for_x = 2

            let leftFloat = left.__float
            left = left.__parent.__proxy

            let rightFloat = right.__float
            right = right.__parent.__proxy

            if(self.parent === left){
                if(leftFloat === AnchorLine.Left){
                    self.AX = ()=>{return this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.Right){
                    self.AX = ()=>{return left.AWidth + this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.AX = ()=>{return left.AWidth / 2 + this.__proxy.leftMargin}
                }
            } else {
                if(leftFloat === AnchorLine.Left){
                    self.AX = ()=>{return left.AX + this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.Right){
                    self.AX = ()=>{return left.AX + left.AWidth + this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.AX = ()=>{return left.AX + left.AWidth / 2 + this.__proxy.leftMargin}
                }
            }

            if(self.parent === right){
                if(rightFloat === AnchorLine.Left){
                    self.AWidth = ()=>{return -this.__proxy.rightMargin - self.AX}
                } else if(rightFloat === AnchorLine.Right){
                    self.AWidth = ()=>{return right.AWidth - this.__proxy.rightMargin - self.AX}
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.AWidth = ()=>{return right.AWidth / 2 - this.__proxy.rightMargin - self.AX}
                }
            } else {
                if(rightFloat === AnchorLine.Left){
                    self.AWidth = ()=>{return right.AX - this.__proxy.rightMargin - self.AX}
                } else if(rightFloat === AnchorLine.Right){
                    self.AWidth = ()=>{return right.AX + right.AWidth - this.__proxy.rightMargin - self.AX}
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.AWidth = ()=>{return right.AX + right.AWidth / 2 - this.__proxy.rightMargin - self.AX}
                }
            }
        } else if(self && left){
            this.__for_x = 1

            let leftFloat = left.__float
            left = left.__parent.__proxy

            if(self.parent === left){
                if(leftFloat === AnchorLine.Left){
                    self.AX = ()=>{return this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.Right){
                    self.AX = ()=>{return left.AWidth + this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.AX = ()=>{return left.AWidth / 2 + this.__proxy.leftMargin}
                }
            } else {
                if(leftFloat === AnchorLine.Left){
                    self.AX = ()=>{return left.AX + this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.Right){
                    self.AX = ()=>{return left.AX + left.AWidth + this.__proxy.leftMargin}
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.AX = ()=>{return left.AX + left.AWidth / 2 + this.__proxy.leftMargin}
                }            
            }
        } else if(self && right){
            this.__for_x = 1

            let rightFloat = right.__float
            right = right.__parent.__proxy

            if(self.parent === right){
                if(rightFloat === AnchorLine.Left){
                    self.AX = ()=>{return -self.AWidth - this.__proxy.rightMargin}
                } else if(rightFloat === AnchorLine.Right){
                    self.AX = ()=>{return right.AWidth - self.AWidth - this.__proxy.rightMargin}
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.AX = ()=>{return right.AWidth / 2 - self.AWidth - this.__proxy.rightMargin}
                }
            } else {
                if(rightFloat === AnchorLine.Left){
                    self.AX = ()=>{return right.AX - self.AWidth - this.__proxy.rightMargin}
                } else if(rightFloat === AnchorLine.Right){
                    self.AX = ()=>{return right.AX + right.AWidth - self.AWidth - this.__proxy.rightMargin}
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.AX = ()=>{return right.AX + right.AWidth / 2 - self.AWidth - this.__proxy.rightMargin}
                }              
            }
        }
    }

    SLOT_topChanged(oldValue, newValue){
        if(newValue && newValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_y === 2){
                        self.AY = self.AY
                        self.AHeight = self.AHeight
                    } else if(this.__for_y === 1){
                        self.AY = self.AY
                    }
                }
                this.top = null
            }
            destructionFunc.meta = {
                name: 'anchors_top__destruction',
                destruction: true,
                parent: newValue.__parent.__proxy
            }
            newValue.__parent.__proxy.__addLink()
            if(newValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = newValue.__parent.__proxy
                newValue.__parent.__proxy['Component.destruction'].connect(destructionFunc)
            }
        }
        if(oldValue && oldValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy
                if(self){ 
                    if(this.__for_y === 2){
                        self.AY = self.AY
                        self.AHeight = self.AHeight
                    } else if(this.__for_y === 1){
                        self.AY = self.AY
                    }
                }
                this.top = null
            }
            destructionFunc.meta = {
                name: 'anchors_top__destruction',
                destruction: true,
                parent: oldValue.__parent.__proxy
            }
            if(oldValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = oldValue.__parent.__proxy
                oldValue.__parent.__proxy['Component.destruction'].disconnect(destructionFunc)
            }
            oldValue.__parent.__proxy.__removeLink()
        }

        this.onTopBottomChanged()
    }

    SLOT_bottomChanged(oldValue, newValue){
        this.onTopBottomChanged()
        
        if(newValue && newValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_y === 2){
                        self.AY = self.AY
                        self.AHeight = self.AHeight
                    } else if(this.__for_y === 1){
                        self.AY = self.AY
                    }
                }
                this.bottom = null
            }
            destructionFunc.meta = {
                name: 'anchors_bottom__destruction',
                destruction: true,
                parent: newValue.__parent.__proxy
            }
            newValue.__parent.__proxy.__addLink()
            if(newValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = newValue.__parent.__proxy
                newValue.__parent.__proxy['Component.destruction'].connect(destructionFunc)
            }
        }
        if(oldValue && oldValue.__parent.__proxy instanceof JQModules.QtQml.QObject){
            let destructionFunc = ()=>{
                let self = this.__parent.__proxy 
                if(self){
                    if(this.__for_y === 2){
                        self.AY = self.AY
                        self.AHeight = self.AHeight
                    } else if(this.__for_y === 1){
                        self.AY = self.AY
                    }
                    this.bottom = null
                }
                
            }
            destructionFunc.meta = {
                name: 'anchors_bottom__destruction',
                destruction: true,
                parent: oldValue.__parent.__proxy
            }
            if(oldValue.__parent.__proxy instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = oldValue.__parent.__proxy
                oldValue.__parent.__proxy['Component.destruction'].disconnect(destructionFunc)
            }
            oldValue.__parent.__proxy.__removeLink()
        }
    }

    onTopBottomChanged(){
        let top = this.__proxy.top
        let bottom = this.__proxy.bottom
        let self = this.__parent.__proxy

        this.__for_y = 0

        if(self && top && bottom){
            this.__for_y = 2

            let topFloat = top.__float
            top = top.__parent.__proxy

            let bottomFloat = bottom.__float
            bottom = bottom.__parent.__proxy

            if(self.parent === top){
                if(topFloat === AnchorLine.Top){
                    self.AY = ()=>{return this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.Bottom){
                    self.AY = ()=>{return top.AHeight + this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.AY = ()=>{return top.AHeight / 2 + this.__proxy.topMargin}
                }
            } else {
                if(topFloat === AnchorLine.Top){
                    self.AY = ()=>{return top.AY + this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.Bottom){
                    self.AY = ()=>{return top.AY + top.AHeight + this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.AY = ()=>{return top.AY + top.AHeight / 2 + this.__proxy.topMargin}
                }
            }
            
            if(self.parent === bottom){
                if(bottomFloat === AnchorLine.Top){
                    self.AHeight = ()=>{return -this.__proxy.bottomMargin - self.AY}
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.AHeight = ()=>{return bottom.AHeight - this.__proxy.bottomMargin - self.AY}
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.AHeight = ()=>{return bottom.AHeight / 2 - this.__proxy.bottomMargin - self.AY}
                }
            } else {
                if(bottomFloat === AnchorLine.Top){
                    self.AHeight = ()=>{return bottom.AY - this.__proxy.bottomMargin - self.AY}
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.AHeight = ()=>{return bottom.AY + bottom.AHeight - this.__proxy.bottomMargin - self.AY}
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.AHeight = ()=>{return bottom.AY + bottom.AHeight / 2 - this.__proxy.bottomMargin - self.AY}
                }
            }
        } else if(self && top){
            this.__for_y = 1

            let topFloat = top.__float
            top = top.__parent.__proxy

            if(self.parent === top){
                if(topFloat === AnchorLine.Top){
                    self.AY = ()=>{return this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.Bottom){
                    self.AY = ()=>{return top.AHeight + this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.AY = ()=>{return top.AHeight / 2 + this.__proxy.topMargin}
                }
            } else {
                if(topFloat === AnchorLine.Top){
                    self.AY = ()=>{return top.AY + this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.Bottom){
                    self.AY = ()=>{return top.AY + top.AHeight + this.__proxy.topMargin}
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.AY = ()=>{return top.AY + top.AHeight / 2 + this.__proxy.topMargin}
                }               
            }
        } else if(self && bottom){
            this.__for_y = 1

            let bottomFloat = bottom.__float
            bottom = bottom.__parent.__proxy

            if(self.parent === bottom){
                if(bottomFloat === AnchorLine.Top){
                    self.AY = ()=>{return -self.AHeight - this.__proxy.bottomMargin}
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.AY = ()=>{return bottom.AHeight - self.AHeight - this.__proxy.bottomMargin}
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.AY = ()=>{return bottom.AHeight / 2 - self.AHeight - this.__proxy.bottomMargin}
                }
            } else {
                if(bottomFloat === AnchorLine.Top){
                    self.AY = ()=>{return bottom.AY - self.AHeight - this.__proxy.bottomMargin}
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.AY = ()=>{return bottom.AY + bottom.AHeight - self.AHeight - this.__proxy.bottomMargin}
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.AY = ()=>{return bottom.AY + bottom.AHeight / 2 - self.AHeight - this.__proxy.bottomMargin}
                }               
            }
        }
    }
}



module.exports = Anchors