const usbDevice = 1;
const bluetoothDevice = 2;

const ipInt = require('ip-to-int');
let libnetworkInterfaces;

if (process.platform === "win32") {
    libnetworkInterfaces = require('./route_tools_module_win')
} else {
    libnetworkInterfaces = require('./route_tools_module_linux')

}
module.exports = libnetworkInterfaces;