const libnetworkInterfaces = require('./build/Release/route_tools');
const ipInt = require('ip-to-int');
const usbDevice = 1;
const bluetoothDevice = 2;

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
  }

function tryAddRoute(device, caiNetwork , caiNetworkCIDR) {
    let result = -1;
    return new Promise( (resolve, reject) => {
        let findAdapter = -1;
        findAdapter = libnetworkInterfaces.findAdapter(device);
        if (findAdapter > 0) {
                const gateway = libnetworkInterfaces.getGatewayAdapter(findAdapter);
                if (gateway > 0) {
                    const ip = ipInt(gateway).toIP().toString();
                    const oct = ip.split('.');
                    const ipString = oct[3] + '.' + oct[2] + '.' + oct[1] + '.' + oct[0];
                    result = libnetworkInterfaces.addRoute(findAdapter, caiNetwork, caiNetworkCIDR, ipString);
                }
        }
        return resolve(result);        
    });
}


function setIpRoute(device, caiNetwork , caiNetworkCIDR, n=5) {
    return new Promise(async (resolve, reject)=>{
        let result = -1;
        for(let i=0; i < n; i++) {
            const vsleep = await sleep( i * 1000);
            result = await tryAddRoute(device, caiNetwork, caiNetworkCIDR);
            if(result === 0) {
                return resolve(result);
            }
        }
        return reject(result);
    });
}

function deleteNetRoute(device, caiNetwork, caiNetworkCIDR) {
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

function deleteGroupRoute(device, caiGroupNetwork, caiNetworkCIDR)  {
    return new Promise((resolve) => {
        setTimeout(() => {
            const adapterFoundId = libnetworkInterfaces.findAdapter(device);
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

