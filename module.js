const usbDevice = 1;
const bluetoothDevice = 2;

const ipInt = require('ip-to-int');
/* console.log(process.platform === "win32" );
 */
let libnetworkInterfaces;

if (process.platform === "win32") {
    libnetworkInterfaces = require('./module_win')
} else {
    libnetworkInterfaces = require('./module_linux')

}
module.exports = libnetworkInterfaces;