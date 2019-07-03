const libnetworkInterfaces = require('./build/Release/module');
const ipInt = require('ip-to-int');
const usbDevice = 1;
const bluetoothDevice = 2;
 function setIpRoute(device, caiNetwork , caiNetworkCIDR) {
    let result = -1;
    return new Promise( (resolve, reject) => {
        let findAdapter = -1;
        setTimeout( () => {
            findAdapter = libnetworkInterfaces.findAdapter(device);
            if (findAdapter > 0) {
                setTimeout(() => {
                    findAdapter = JSON.stringify(findAdapter);
                    findAdapter = JSON.parse(findAdapter);        
                    const gateway = libnetworkInterfaces.getGatewayAdapter(findAdapter);
                    if (gateway > 0) {
                        const ip = ipInt(gateway).toIP().toString();
                        const oct = ip.split('.');
                        const ipString = oct[3] + '.' + oct[2] + '.' + oct[1] + '.' + oct[0];
                        result = libnetworkInterfaces.addRoute(findAdapter, caiNetwork, caiNetworkCIDR, ipString);
                        resolve(result);
                    }
                }, 1500);
            }
        }, 1000 );
    });
}

function deleteNetRoute(device, caiNetwork) {
    let result = -1;
    return new Promise( (resolve, reject) => {
        let findAdapter = -1;
        setTimeout(() => {
            findAdapter = libnetworkInterfaces.findAdapter(device);
            const caiNetworkAddress = Number(caiNetwork);
            const listNetWorks = [ caiNetworkAddress , caiNetworkAddress + usbDevice, caiNetworkAddress + bluetoothDevice ];
            if (findAdapter > 0) {
                listNetWorks.forEach(network => {
                    const fullNetworkAddress = `${network}.0.0.0`;
                    result = libnetworkInterfaces.deleteRoute(findAdapter, fullNetworkAddress);
                });
                resolve(result);
            }
        }, 1000 );
    });
}

function deleteGroupRoute(device, caiGroupNetwork)  {
    return new Promise((resolve) => {
        setTimeout(() => {
            const adapterFoundId = libnetworkInterfaces.findAdapter(device);
            adapterFoundId = JSON.stringify(adapterFoundId);
            adapterFoundId = JSON.parse(adapterFoundId);
            if (adapterFoundId > 0) {
                const fullNetworkAddress = `${caiGroupNetwork}.0.0.0`;
                const routeDeleted = libnetworkInterfaces.deleteRoute(adapterFoundId, fullNetworkAddress);
                resolve(routeDeleted);
            }
            resolve(false);
        }, 1000);
    });
}
libnetworkInterfaces.setIpRoute = setIpRoute;
libnetworkInterfaces.deleteNetRoute = deleteNetRoute;
libnetworkInterfaces.deleteGroupRoute = deleteGroupRoute;

module.exports = libnetworkInterfaces;

