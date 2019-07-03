const libnetworkInterfaces = require('./build/Release/module');
const ipInt = require('ip-to-int');
const usbDevice = 1;
const bluetoothDevice = 2;


function setIpRoute(device, caiNetwork , caiNetworkCIDR) {
    const destinationAddres = caiNetwork + '/' + caiNetworkCIDR;
    let result = -1;
    return new Promise( (resolve, reject) => {
        const adapterInterfaces = libnetworkInterfaces.listAdapterInterfaces();
        filteredInterfaces = []
        adapterInterfaces.forEach(interface => {
            if(interface.ifName.includes(device)) {
                filteredInterfaces.push(interface);
            }
        });
        filteredInterfaces.forEach(interface => {
            result = libnetworkInterfaces.addRoute(destinationAddres, interface.gateway);
            resolve(result);
        });
    });
}