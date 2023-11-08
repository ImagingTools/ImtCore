const path = require('path')

module.exports = {
    entry: "./core/core.js",
    output: {
        path: path.resolve(__dirname, 'core/dist'),
    },
}