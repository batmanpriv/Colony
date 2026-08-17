const tlsSettings = () => {
    return {
        brave: {
            ciphers: [
                "ECDHE-ECDSA-AES128-GCM-SHA256", 
                "ECDHE-RSA-AES128-GCM-SHA256"
            ],
            sigalgs: [
                "ecdsa_secp256r1_sha256", 
                "rsa_pss_rsae_sha256"
            ]
        },
        chrome: {
            ciphers: [
                "TLS_AES_128_GCM_SHA256", 
                "TLS_AES_256_GCM_SHA384"
            ],
            sigalgs: [
                "rsa_pss_rsae_sha256", 
                "ecdsa_secp384r1_sha384"
            ]
        },
        edge: {
            ciphers: [
                "TLS_AES_128_GCM_SHA256", 
                "TLS_AES_256_GCM_SHA384"
            ],
            sigalgs: [
                "rsa_pss_rsae_sha256", 
                "ecdsa_secp384r1_sha384"
            ]
        },
        firefox: {
            ciphers: [
                "TLS_AES_128_GCM_SHA256", 
                "TLS_CHACHA20_POLY1305_SHA256"
            ],
            sigalgs: [
                "rsa_pss_rsae_sha256", 
                "ecdsa_secp256r1_sha256"
            ]
        },
        mobile: {
            ciphers: [
                "TLS_AES_128_GCM_SHA256", 
                "TLS_CHACHA20_POLY1305_SHA256"
            ],
            sigalgs: [
                "rsa_pss_rsae_sha256", 
                "ecdsa_secp256r1_sha256"
            ]
        },
        opera: {
            ciphers: [
                "TLS_AES_128_GCM_SHA256", 
                "TLS_AES_256_GCM_SHA384"
            ],
            sigalgs: [
                "rsa_pss_rsae_sha256", 
                "ecdsa_secp384r1_sha384"
            ]
        },
        operagx: {
            ciphers: [
                "TLS_AES_128_GCM_SHA256", 
                "ECDHE-RSA-AES256-GCM-SHA384"
            ],
            sigalgs: [
                "rsa_pss_rsae_sha256", 
                "ecdsa_secp256r1_sha256"
            ]
        },
        safari: {
            ciphers: [
                "ECDHE-ECDSA-AES128-GCM-SHA256", 
                "TLS_AES_128_GCM_SHA256"
            ],
            sigalgs: [
                "ecdsa_secp256r1_sha256", 
                "rsa_pss_rsae_sha256"
            ]
        }
    };
};

module.exports = tlsSettings;