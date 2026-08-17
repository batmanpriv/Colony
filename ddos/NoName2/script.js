function abc(hardwareConcurrency, deviceMemory, m) {

    if (navigator) {
        /*if (navigator.prototype) {
            navigator.prototype.canShare = () => false
            navigator.prototype.share = () => false

            navigator.canShare = () => false
            navigator.share = () => false
        }*/
    }

    if (navigator.mediaDevices) {
        delete navigator.mediaDevices.getUserMedia
        delete navigator.webkitGetUserMedia
        delete navigator.mozGetUserMedia
        delete navigator.getUserMedia
        delete webkitRTCPeerConnection
        delete RTCPeerConnection
        delete MediaStreamTrack
    }

    const cache = {
        Reflect: {
            get: Reflect.get.bind(Reflect),
            apply: Reflect.apply.bind(Reflect),
        },
        // Used in `makeNativeString`
        nativeToStringStr: `${Function.toString}`, // => `function toString() { [native code] }`
    };
    /**
     * @param masterObject Object to override.
     * @param propertyName Property to override.
     * @param proxyHandler Proxy handled with the new value.
     */
    function overridePropertyWithProxy(masterObject, propertyName, proxyHandler) {
        const originalObject = masterObject[propertyName];
        const proxy = new Proxy(masterObject[propertyName], stripProxyFromErrors(proxyHandler));
        redefineProperty(masterObject, propertyName, { value: proxy });
        redirectToString(proxy, originalObject);
    }
    const prototypeProxyHandler = {
        setPrototypeOf: (target, newProto) => {
            try {
                throw new TypeError('Cyclic __proto__ value');
            }
            catch (e) {
                const oldStack = e.stack;
                const oldProto = Object.getPrototypeOf(target);
                Object.setPrototypeOf(target, newProto);
                try {
                    // shouldn't throw if prototype is okay, will throw if there is a prototype cycle (maximum call stack size exceeded).
                    target['nonexistentpropertytest'];
                    return true;
                }
                catch (err) {
                    Object.setPrototypeOf(target, oldProto);
                    if (oldStack.includes('Reflect.setPrototypeOf'))
                        return false;
                    const newError = new TypeError('Cyclic __proto__ value');
                    const stack = oldStack.split('\n');
                    newError.stack = [stack[0], ...stack.slice(2)].join('\n');
                    throw newError;
                }
            }
        },
    };
    /**
     * @param masterObject Object to override.
     * @param propertyName Property to override.
     * @param proxyHandler Proxy handled with getter handler.
     */
    function overrideGetterWithProxy(masterObject, propertyName, proxyHandler) {
        const fn = Object.getOwnPropertyDescriptor(masterObject, propertyName).get;
        const fnStr = fn.toString; // special getter function string
        const proxyObj = new Proxy(fn, {
            ...stripProxyFromErrors(proxyHandler),
            ...prototypeProxyHandler,
        });
        redefineProperty(masterObject, propertyName, { get: proxyObj });
        redirectToString(proxyObj, fnStr);
    }
    /**
     * @param instance Instance to override.
     * @param overrideObj New instance values.
     */
    // eslint-disable-next-line no-unused-vars
    function overrideInstancePrototype(instance, overrideObj) {
        Object.keys(overrideObj).forEach((key) => {
            if (!(overrideObj[key] === null)) {
                try {
                    overrideGetterWithProxy(Object.getPrototypeOf(instance), key, makeHandler().getterValue(overrideObj[key]));
                }
                catch (e) {
                    //console.error(`Could not override property: ${key} on ${instance}. Reason: ${e.message} `); // some fingerprinting services can be listening
                    return false;
                }
            }
        });
    }
    function redirectToString(proxyObj, originalObj) {
        const handler = {
            setPrototypeOf: (target, newProto) => {
                try {
                    throw new TypeError('Cyclic __proto__ value');
                }
                catch (e) {
                    if (e.stack.includes('Reflect.setPrototypeOf'))
                        return false;
                    // const stack = e.stack.split('\n');
                    // e.stack = [stack[0], ...stack.slice(2)].join('\n');
                    throw e;
                }
            },
            apply(target, ctx) {
                // This fixes e.g. `HTMLMediaElement.prototype.canPlayType.toString + ""`
                if (ctx === Function.prototype.toString) {
                    return makeNativeString('toString');
                }
                // `toString` targeted at our proxied Object detected
                if (ctx === proxyObj) {
                    // Return the toString representation of our original object if possible
                    return makeNativeString(proxyObj.name);
                }
                // Check if the toString prototype of the context is the same as the global prototype,
                // if not indicates that we are doing a check across different windows., e.g. the iframeWithdirect` test case
                const hasSameProto = Object.getPrototypeOf(Function.prototype.toString).isPrototypeOf(ctx.toString); // eslint-disable-line no-prototype-builtins
                if (!hasSameProto) {
                    // Pass the call on to the local Function.prototype.toString instead
                    return ctx.toString();
                }
                if (Object.getPrototypeOf(ctx) === proxyObj) {
                    try {
                        return target.call(ctx);
                    }
                    catch (err) {
                        err.stack = err.stack.replace('at Object.toString (', 'at Function.toString (');
                        throw err;
                    }
                }
                return target.call(ctx);
            },
            get: function (target, prop, receiver) {
                if (prop === 'toString') {
                    return new Proxy(target.toString, {
                        apply: function (tget, thisArg, argumentsList) {
                            try {
                                return tget.bind(thisArg)(...argumentsList);
                            }
                            catch (err) {
                                if (Object.getPrototypeOf(thisArg) === tget) {
                                    err.stack = err.stack.replace('at Object.toString (', 'at Function.toString (');
                                }
                                throw err;
                            }
                        }
                    });
                }
                return Reflect.get(...arguments);
            }
        };
        const toStringProxy = new Proxy(Function.prototype.toString, stripProxyFromErrors(handler));
        redefineProperty(Function.prototype, 'toString', {
            value: toStringProxy,
        });
    }
    function makeNativeString(name = '') {
        return cache.nativeToStringStr.replace('toString', name || '');
    }
    function redefineProperty(masterObject, propertyName, descriptorOverrides = {}) {
        return Object.defineProperty(masterObject, propertyName, {
            // Copy over the existing descriptors (writable, enumerable, configurable, etc)
            ...(Object.getOwnPropertyDescriptor(masterObject, propertyName) || {}),
            // Add our overrides (e.g. value, get())
            ...descriptorOverrides,
        });
    }
    function stripProxyFromErrors(handler) {
        const newHandler = {};
        // We wrap each trap in the handler in a try/catch and modify the error stack if they throw
        const traps = Object.getOwnPropertyNames(handler);
        traps.forEach((trap) => {
            newHandler[trap] = function () {
                try {
                    // Forward the call to the defined proxy handler
                    return handler[trap].apply(this, arguments || []); //eslint-disable-line
                }
                catch (err) {
                    // Stack traces differ per browser, we only support chromium based ones currently
                    if (!err || !err.stack || !err.stack.includes(`at `)) {
                        throw err;
                    }
                    // When something throws within one of our traps the Proxy will show up in error stacks
                    // An earlier implementation of this code would simply strip lines with a blacklist,
                    // but it makes sense to be more surgical here and only remove lines related to our Proxy.
                    // We try to use a known "anchor" line for that and strip it with everything above it.
                    // If the anchor line cannot be found for some reason we fall back to our blacklist approach.
                    const stripWithBlacklist = (stack, stripFirstLine = true) => {
                        const blacklist = [
                            `at Reflect.${trap} `,
                            `at Object.${trap} `,
                            `at Object.newHandler.<computed> [as ${trap}] `,
                            `at newHandler.<computed> [as ${trap}] `, // also caused by this wrapper :p
                        ];
                        return (err.stack
                            .split('\n')
                            // Always remove the first (file) line in the stack (guaranteed to be our proxy)
                            .filter((line, index) => !(index === 1 && stripFirstLine))
                            // Check if the line starts with one of our blacklisted strings
                            .filter((line) => !blacklist.some((bl) => line.trim().startsWith(bl)))
                            .join('\n'));
                    };
                    const stripWithAnchor = (stack, anchor) => {
                        const stackArr = stack.split('\n');
                        anchor = anchor || `at Object.newHandler.<computed> [as ${trap}] `; // Known first Proxy line in chromium
                        const anchorIndex = stackArr.findIndex((line) => line.trim().startsWith(anchor));
                        if (anchorIndex === -1) {
                            return false; // 404, anchor not found
                        }
                        // Strip everything from the top until we reach the anchor line
                        // Note: We're keeping the 1st line (zero index) as it's unrelated (e.g. `TypeError`)
                        stackArr.splice(1, anchorIndex);
                        return stackArr.join('\n');
                    };
                    const oldStackLines = err.stack.split('\n');
                    Error.captureStackTrace(err);
                    const newStackLines = err.stack.split('\n');
                    err.stack = [newStackLines[0], oldStackLines[1], ...newStackLines.slice(1)].join('\n');
                    if ((err.stack || '').includes('toString (')) {
                        err.stack = stripWithBlacklist(err.stack, false);
                        throw err;
                    }
                    // Try using the anchor method, fallback to blacklist if necessary
                    err.stack = stripWithAnchor(err.stack) || stripWithBlacklist(err.stack);
                    throw err; // Re-throw our now sanitized error
                }
            };
        });
        return newHandler;
    }

    function overrideWebGl(webGl) {
        // try to override WebGl
        try {
            // Remove traces of our Proxy
            const stripErrorStack = (stack) => stack
                .split('\n')
                .filter((line) => !line.includes('at Object.apply'))
                .filter((line) => !line.includes('at Object.get'))
                .join('\n');
            const getParameterProxyHandler = {
                ...prototypeProxyHandler,
                get(target, key) {
                    try {
                        return Reflect.get(target, key);
                    }
                    catch (err) {
                        err.stack = stripErrorStack(err.stack);
                        throw err;
                    }
                },
                apply(target, thisArg, args) {
                    const param = (args || [])[0];
                    // UNMASKED_VENDOR_WEBGL
                    if (param === 37445) {
                        return webGl.vendor;
                    }
                    // UNMASKED_RENDERER_WEBGL
                    if (param === 37446) {
                        return webGl.renderer;
                    }
                    try {
                        return cache.Reflect.apply(target, thisArg, args);
                    }
                    catch (err) {
                        err.stack = stripErrorStack(err.stack);
                        throw err;
                    }
                },
            };
            const addProxy = (obj, propName) => {
                overridePropertyWithProxy(obj, propName, getParameterProxyHandler);
            };
            addProxy(WebGLRenderingContext.prototype, 'getParameter');
            addProxy(WebGL2RenderingContext.prototype, 'getParameter');
        }
        catch (err) {
            console.warn(err);
        }
    }

    function makeHandler() {
        return {
            // Used by simple `navigator` getter evasions
            getterValue: (value) => ({
                apply(target, ctx, args) {
                    // Let's fetch the value first, to trigger and escalate potential errors
                    // Illegal invocations like `navigator.__proto__.vendor` will throw here
                    const ret = cache.Reflect.apply(...arguments); // eslint-disable-line
                    if (args && args.length === 0) {
                        return value;
                    }
                    return ret;
                },
            }),
        };
    }

    function overrideUserAgentData(userAgentData) {
        const { ...highEntropyValues } = userAgentData;
        // Override basic properties
        const getHighEntropyValues = {
            // eslint-disable-next-line
            apply: async function (target, ctx, args) {
                // Just to throw original validation error
                // Remove traces of our Proxy
                const stripErrorStack = (stack) => stack
                    .split('\n')
                    .filter((line) => !line.includes('at Object.apply'))
                    .filter((line) => !line.includes('at Object.get'))
                    .join('\n');
                try {
                    if (!args || !args.length) {
                        return target.apply(ctx, args);
                    }
                    const [hints] = args;
                    await target.apply(ctx, args);
                    // If the codec is not in our collected data use
                    const data = {};
                    hints.forEach((hint) => {
                        data[hint] = highEntropyValues[hint];
                    });
                    return data;
                }
                catch (err) {
                    err.stack = stripErrorStack(err.stack);
                    throw err;
                }
            },
        };
        if (self.navigator.userAgentData) { // Firefox does not contain this property - to be fixed 
            overridePropertyWithProxy(Object.getPrototypeOf(self.navigator.userAgentData), 'getHighEntropyValues', getHighEntropyValues);
            const { brands, mobile, platform } = userAgentData;
            overrideInstancePrototype(self.navigator.userAgentData, { brands, mobile, platform });
        }
    }

    const WEBGL_RENDERERS = ['ANGLE (NVIDIA Quadro 2000M Direct3D11 vs_5_0 ps_5_0)', 'ANGLE (NVIDIA Quadro K420 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA Quadro 2000M Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA Quadro K2000M Direct3D11 vs_5_0 ps_5_0)', 'ANGLE (Intel(R) HD Graphics Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) HD Graphics Family Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (ATI Radeon HD 3800 Series Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) HD Graphics 4000 Direct3D11 vs_5_0 ps_5_0)', 'ANGLE (Intel(R) HD Graphics 4000 Direct3D11 vs_5_0 ps_5_0)', 'ANGLE (AMD Radeon R9 200 Series Direct3D11 vs_5_0 ps_5_0)', 'ANGLE (Intel(R) HD Graphics Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) HD Graphics Family Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) HD Graphics Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) HD Graphics Family Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) HD Graphics 4000 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) HD Graphics 3000 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Mobile Intel(R) 4 Series Express Chipset Family Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) G33/G31 Express Chipset Family Direct3D9Ex vs_0_0 ps_2_0)', 'ANGLE (Intel(R) Graphics Media Accelerator 3150 Direct3D9Ex vs_0_0 ps_2_0)', 'ANGLE (Intel(R) G41 Express Chipset Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce 6150SE nForce 430 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) HD Graphics 4000)', 'ANGLE (Mobile Intel(R) 965 Express Chipset Family Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) HD Graphics Family)', 'ANGLE (NVIDIA GeForce GTX 760 Direct3D11 vs_5_0 ps_5_0)', 'ANGLE (NVIDIA GeForce GTX 760 Direct3D11 vs_5_0 ps_5_0)', 'ANGLE (NVIDIA GeForce GTX 760 Direct3D11 vs_5_0 ps_5_0)', 'ANGLE (AMD Radeon HD 6310 Graphics Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) Graphics Media Accelerator 3600 Series Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) G33/G31 Express Chipset Family Direct3D9 vs_0_0 ps_2_0)', 'ANGLE (AMD Radeon HD 6320 Graphics Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) G33/G31 Express Chipset Family (Microsoft Corporation - WDDM 1.0) Direct3D9Ex vs_0_0 ps_2_0)', 'ANGLE (Intel(R) G41 Express Chipset)', 'ANGLE (ATI Mobility Radeon HD 5470 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) Q45/Q43 Express Chipset Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce 310M Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) G41 Express Chipset Direct3D9 vs_3_0 ps_3_0)', 'ANGLE (Mobile Intel(R) 45 Express Chipset Family (Microsoft Corporation - WDDM 1.1) Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce GT 440 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (ATI Radeon HD 4300/4500 Series Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (AMD Radeon HD 7310 Graphics Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) HD Graphics)', 'ANGLE (Intel(R) 4 Series Internal Chipset Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (AMD Radeon(TM) HD 6480G Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (ATI Radeon HD 3200 Graphics Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (AMD Radeon HD 7800 Series Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) G41 Express Chipset (Microsoft Corporation - WDDM 1.1) Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce 210 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce GT 630 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (AMD Radeon HD 7340 Graphics Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) 82945G Express Chipset Family Direct3D9 vs_0_0 ps_2_0)', 'ANGLE (NVIDIA GeForce GT 430 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce 7025 / NVIDIA nForce 630a Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) Q35 Express Chipset Family Direct3D9Ex vs_0_0 ps_2_0)', 'ANGLE (Intel(R) HD Graphics 4600 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (AMD Radeon HD 7520G Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (AMD 760G (Microsoft Corporation WDDM 1.1) Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce GT 220 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce 9500 GT Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) HD Graphics Family Direct3D9 vs_3_0 ps_3_0)', 'ANGLE (Intel(R) Graphics Media Accelerator HD Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce 9800 GT Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) Q965/Q963 Express Chipset Family (Microsoft Corporation - WDDM 1.0) Direct3D9Ex vs_0_0 ps_2_0)', 'ANGLE (NVIDIA GeForce GTX 550 Ti Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) Q965/Q963 Express Chipset Family Direct3D9Ex vs_0_0 ps_2_0)', 'ANGLE (AMD M880G with ATI Mobility Radeon HD 4250 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce GTX 650 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (ATI Mobility Radeon HD 5650 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (ATI Radeon HD 4200 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (AMD Radeon HD 7700 Series Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) G33/G31 Express Chipset Family)', 'ANGLE (Intel(R) 82945G Express Chipset Family Direct3D9Ex vs_0_0 ps_2_0)', 'ANGLE (SiS Mirage 3 Graphics Direct3D9Ex vs_2_0 ps_2_0)', 'ANGLE (NVIDIA GeForce GT 430)', 'ANGLE (AMD RADEON HD 6450 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (ATI Radeon 3000 Graphics Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) 4 Series Internal Chipset Direct3D9 vs_3_0 ps_3_0)', 'ANGLE (Intel(R) Q35 Express Chipset Family (Microsoft Corporation - WDDM 1.0) Direct3D9Ex vs_0_0 ps_2_0)', 'ANGLE (NVIDIA GeForce GT 220 Direct3D9 vs_3_0 ps_3_0)', 'ANGLE (AMD Radeon HD 7640G Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (AMD 760G Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (AMD Radeon HD 6450 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce GT 640 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce 9200 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce GT 610 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (AMD Radeon HD 6290 Graphics Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (ATI Mobility Radeon HD 4250 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce 8600 GT Direct3D9 vs_3_0 ps_3_0)', 'ANGLE (ATI Radeon HD 5570 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (AMD Radeon HD 6800 Series Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) G45/G43 Express Chipset Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (ATI Radeon HD 4600 Series Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA Quadro NVS 160M Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) HD Graphics 3000)', 'ANGLE (NVIDIA GeForce G100)', 'ANGLE (AMD Radeon HD 8610G + 8500M Dual Graphics Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Mobile Intel(R) 4 Series Express Chipset Family Direct3D9 vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce 7025 / NVIDIA nForce 630a (Microsoft Corporation - WDDM) Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) Q965/Q963 Express Chipset Family Direct3D9 vs_0_0 ps_2_0)', 'ANGLE (AMD RADEON HD 6350 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (ATI Radeon HD 5450 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce 9500 GT)', 'ANGLE (AMD Radeon HD 6500M/5600/5700 Series Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Mobile Intel(R) 965 Express Chipset Family)', 'ANGLE (NVIDIA GeForce 8400 GS Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (Intel(R) HD Graphics Direct3D9 vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce GTX 560 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce GT 620 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce GTX 660 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (AMD Radeon(TM) HD 6520G Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA GeForce GT 240 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (AMD Radeon HD 8240 Direct3D9Ex vs_3_0 ps_3_0)', 'ANGLE (NVIDIA Quadro NVS 140M)', 'ANGLE (Intel(R) Q35 Express Chipset Family Direct3D9 vs_0_0 ps_2_0)']
        .filter(x => x.indexOf("NVIDIA") != -1);

    overrideWebGl({ vendor: "Google Inc. (NVIDIA Corporation)", renderer: WEBGL_RENDERERS[~~(m * WEBGL_RENDERERS.length - 1)] })
    overrideInstancePrototype(navigator, {
        deviceMemory,
        platform: "Linux x86_64",
        hardwareConcurrency
    })

    if ('connection' in navigator) {
        overrideInstancePrototype(navigator.connection, {
            rtt: 100
        })
    }


    /*overrideUserAgentData({
        "brands": [
            {
                "brand": "Chromium",
                "version": `${fullUa.split('.')[0]}`
            },
            {
                "brand": "Not A(Brand",
                "version": "24"
            },
            {
                "brand": "Google Chrome",
                "version": `${fullUa.split('.')[0]}`
            }
        ],
        "mobile": false,
        "platform": "Linux",
        "architecture": "x86",
        "bitness": "64",
        "fullVersionList": [
            {
                "brand": "Chromium",
                "version": `${fullUa}`
            },
            {
                "brand": "Not A(Brand",
                "version": "24.0.0.0"
            },
            {
                "brand": "Google Chrome",
                "version": `${fullUa}`
            }
        ],
        "model": "",
        "platformVersion": "10.0.0",
        "uaFullVersion": `${fullUa}`
    })*/
    if (self.screen) {
        overrideInstancePrototype(self.screen, {
            availHeight: screen[1] - 20,
            availLeft: 0,
            availTop: 0,
            availWidth: screen[0],
            colorDepth: 24,
            height: screen[1],
            pixelDepth: 24,
            width: screen[0]
        });
    }
    /*let context = {
        BUFFER: null
    }

    function overrideSomething(obj, property, callback) {
        const stripErrorStack = (stack) => stack
            .split('\n')
            .filter((line) => !line.includes('at Object.apply'))
            .filter((line) => !line.includes('at Object.get'))
            .join('\n');
        const getParameterProxyHandler = {
            ...prototypeProxyHandler,
            get(target, key) {
                try {
                    return Reflect.get(target, key);
                }
                catch (err) {
                    err.stack = stripErrorStack(err.stack);
                    throw err;
                }
            },
            apply(target, thisArg, args) {
                return callback(target, thisArg, args)
            },
        };

        overridePropertyWithProxy(obj, property, getParameterProxyHandler);
    }

    /*overrideSomething(AudioBuffer.prototype, 'getChannelData', function (target, thisArg, args) {
        const results_1 = cache.Reflect.apply(target, thisArg, args);
        //
        if (context.BUFFER !== results_1) {
            context.BUFFER = results_1;
            //
            console.log({ results_1 })
            for (let i = 100; i < 100; i++) {
                //results_1[index] = results_1[index] + Math.random() * 0.0000001;
                results_1[i] *= scale
            }
        }

        //
        return results_1;
    })

    overrideSomething(AudioBuffer.prototype, 'copyFromChannel', function (target, thisArg, args) {
        const buffer = thisArg.getChannelData(args[1])   
        let start = args.length > 2 ? args[2] : 0
        for (let i = start; i < buffer.length; i++) {
            args[0][i] = buffer[i]
        }
    })

    overrideSomething(AudioContext.prototype, 'createAnalyser', function (target, thisArg, args) {
        const results_3 = Reflect.apply(target, thisArg, args);
        //
        for (let i = 0; i < arguments[0].length; i ++) {
            arguments[0][i] = arguments[0][i] * scale
        }
        //
        return results_3;
    })

    overrideSomething(OfflineAudioContext.prototype, 'createAnalyser', function (target, thisArg, args) {
        const results_3 = Reflect.apply(target, thisArg, args);
        //
        for (let i = 0; i < arguments[0].length; i ++) {
            arguments[0][i] = arguments[0][i] * scale
        }
        //
        return results_3;
    })*/

}