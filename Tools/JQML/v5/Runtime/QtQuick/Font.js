const { GroupObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

const ownerText = (group) => group.$owner.$updateImplicitSize()

const POINTS_PER_PIXEL = 72 / 96

// QQuickFontValueType: pixelSize and pointSize both read as a size, from whichever was set last
class Font extends GroupObject {
    $info(){
        let v = this.$v
        let weight = v[Font.WEIGHT]
        if(v[Font.BOLD] && weight < 700) weight = 700
        return {
            pixelSize: v[Font.PIXELSIZE],
            family: v[Font.FAMILY],
            weight,
            italic: v[Font.ITALIC],
            underline: v[Font.UNDERLINE],
            strikeout: v[Font.STRIKEOUT],
            capitalization: v[Font.CAPITALIZATION],
            letterSpacing: v[Font.LETTERSPACING],
            wordSpacing: v[Font.WORDSPACING],
        }
    }

    $write(id, value){
        if(value > 0){
            if(id === Font.PIXELSIZE) this.$v[Font.POINTSIZE] = value * POINTS_PER_PIXEL
            if(id === Font.POINTSIZE) this.$v[Font.PIXELSIZE] = Math.round(value / POINTS_PER_PIXEL)
        }
        super.$write(id, value)
    }
}
defineType(Font, {
    props: {
        pixelSize: { kind: 'int', initial: 12, effect: ownerText },
        pointSize: { kind: 'real', initial: 9, effect: ownerText },
        family: { kind: 'string', initial: 'Segoe UI', effect: ownerText },
        bold: { kind: 'bool', effect: ownerText },
        weight: { kind: 'int', initial: 400, effect: ownerText },
        italic: { kind: 'bool', effect: ownerText },
        underline: { kind: 'bool', effect: ownerText },
        strikeout: { kind: 'bool', effect: ownerText },
        capitalization: { kind: 'int', effect: ownerText },
        letterSpacing: { kind: 'real', effect: ownerText },
        wordSpacing: { kind: 'real', effect: ownerText },
        hintingPreference: 'int',
        kerning: { kind: 'bool', initial: true },
        preferShaping: { kind: 'bool', initial: true },
        styleName: 'string',
    },
})
Object.assign(Font, {
    MixedCase: 0, AllUppercase: 1, AllLowercase: 2, SmallCaps: 3, Capitalize: 4,
    Thin: 100, ExtraLight: 200, Light: 300, Normal: 400, Medium: 500, DemiBold: 600, Bold: 700, ExtraBold: 800, Black: 900,
})

module.exports = { Font }
