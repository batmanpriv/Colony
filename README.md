<h1 align="center">🐜 COLONY</h1>

<p align="center">
  <img src="https://github.com/user-attachments/assets/547119b5-71b4-46a9-be7d-3818ed068c05" alt="Colony Banner" width="100%">
</p>

<p align="center">
  <strong><em>"A thriving colony of botnets, DDoS frameworks, payload injectors, and exploitation tools — preserved for education, research, and development."</em></strong>
</p>

<p align="center">
  <strong>Maintainer:</strong> <a href="https://github.com/batmanpriv">@batmanpriv</a>
</p>

---

## ⚠️ DISCLAIMER — READ THIS FIRST

> **THIS REPOSITORY IS FOR EDUCATIONAL, RESEARCH, AND DEVELOPMENT PURPOSES ONLY.**
>
> The code and tools contained herein are **provided "AS IS"** and have been collected from various public sources for the purpose of:
> - **Malware analysis & archaeology**
> - **Security research & threat modeling**
> - **Defensive strategy development**
> - **Academic study & reverse engineering**
> - **Understanding attack vectors to better defend against them**
>
> ### 🔴 IMPORTANT NOTES:
> - **These tools have NOT been tested** for functionality, safety, or effectiveness.
> - **They are NOT guaranteed to work** — many may be incomplete, broken, or outdated.
> - **They are NOT intended for active deployment or real-world use.**
> - **They are for LEARNING and DEVELOPMENT** — study the code, understand the mechanics, and improve your defensive skills.
>
> ### 🚫 PROHIBITED USES:
> - Unauthorized network attacks
> - Compromising systems without explicit permission
> - Any illegal or malicious activity
>
> ### ⚖️ LEGAL:
> - The author assumes **ZERO responsibility** for misuse.
> - By accessing this repository, you agree to use its contents **ethically and legally**.
> - **You are solely responsible** for your actions.
>
> **"With great power comes great responsibility."** — Use this knowledge to **build, protect, and defend**, not to destroy.

---

## 📊 Languages & Technologies

Based on the repository structure, here's the breakdown of programming languages used:

| Language | Used For | Prevalence |
|----------|----------|------------|
| **C** | Bot cores, scanners, loaders, DDoS engines | ⭐⭐⭐⭐⭐ (Most common) |
| **Go** | C2 servers, API backends, scanners, listeners | ⭐⭐⭐⭐ |
| **Python** | Payload generators, automation scripts, RATs | ⭐⭐⭐ |
| **C++** | Windows bots, IRC bots, spreaders, loaders | ⭐⭐⭐ |
| **JavaScript/Node.js** | Browser-based DDoS flooders (Layer 7) | ⭐⭐ |
| **PHP** | C2 web panels, API endpoints, management UIs | ⭐⭐ |
| **Assembly** | Anti-debug, shellcode, low-level tricks | ⭐ |
| **HTML/CSS** | C2 panels, login screens, management dashboards | ⭐ |
| **Bash/Shell** | Build scripts, deployment automation | ⭐⭐ |
| **SQL** | Database schemas for C2 backends | ⭐ |

---

## 🧠 What's Inside?

### 🤖 Botnet Variants (50+)
- **Mirai-based** — AstroMirai, Condi (v4–v7), DivineMirai, YBotV2, ZBOT, Joker, Cosmic, Senpai, Reaper-Sec, KryptomeV6
- **qBot-based** — Cayosin-qBot, Mortem-qBot, Dream-qBot, MODZ V4.5
- **Windows IRC Bots** — LoexBot, urxbot, litmus2-bot, Dbot.v3.1, C_15Pub, EPiC BoT
- **Android & IoT** — android_multy, Fritz!Box, Huawei, Cisco, comtrendvr
- **Custom C2 Frameworks** — Astro C2+Api, Ghost (Python RAT), Hibernet, toxnet, fatebot

### 💥 DDoS Tools
- **HTTP/S Flooders** — SlowLoris, browser2, NoName, NoName2, oldbrowser, Sang-Pematuk
- **Layer 4/Volumetric** — UDP, SYN, ACK, GRE floods integrated in most botnets
- **Advanced attacks** — TLS bypass, RST flood, rapid reset, hybrid attacks

### 🛠️ Supporting Components
- **Loaders** — Telnet brute-forcers, binary droppers, DLR (downloaders)
- **Exploits** — DVR, fiber, zhone, Huawei, Realtek, ASUS, Linksys, GPON, TR-064
- **Encryption** — XOR/RC4 table encoders for payload obfuscation
- **Utilities** — IP scanners, port scanners, password list generators

---

## 📁 Project Structure

```
📦 COLONY
 ┣ 📂 botnet/                  # 50+ botnet variants & C2 frameworks
 ┃ ┣ 📂 Alpha                  # Go-based scanner
 ┃ ┣ 📂 android_multy          # Android bot with multi-arch support
 ┃ ┣ 📂 Astro C2+Api           # C2 panel with REST API + assets
 ┃ ┣ 📂 AstroMirai             # Feature-rich Mirai fork
 ┃ ┣ 📂 botnet-1               # Basic Mirai variant
 ┃ ┣ 📂 botnet-2               # Enhanced Mirai with Telegram integration
 ┃ ┣ 📂 caligulabot            # Bot with custom exploits
 ┃ ┣ 📂 Cayosin-qBot           # qBot variant with Huawei/Realtek scanners
 ┃ ┣ 📂 Cisco                  # Cisco-specific exploit
 ┃ ┣ 📂 comtrendvr             # Comtrend router exploit
 ┃ ┣ 📂 Condi/                 # Condi v4.2, v5, v6, v7, special
 ┃ ┣ 📂 Cosmic                 # Multi-exploit botnet (DVR, fiber, zhone)
 ┃ ┣ 📂 customer_botnet        # Customized CondiV7
 ┃ ┣ 📂 C_15Pub                # Windows-based bot (C++)
 ┃ ┣ 📂 Dbot.v3.1              # Windows DDoS bot
 ┃ ┣ 📂 DivineMiraiVariant     # Mirai with captive portal
 ┃ ┣ 📂 Dream-qBot             # qBot implementation
 ┃ ┣ 📂 DVRBOT                 # DVR-targeting botnet
 ┃ ┣ 📂 EPiC BoT V1            # Windows bot with .exe payloads
 ┃ ┣ 📂 fatebot                # Go-based bot with IRC C2
 ┃ ┣ 📂 Fritz!Box              # Fritz!Box router exploit
 ┃ ┣ 📂 Ghost                  # Python-based RAT
 ┃ ┣ 📂 Gucci-Mirai-Botnet-main
 ┃ ┣ 📂 Hibernet               # Python SOCKS/HTTP proxy bot
 ┃ ┣ 📂 HuaWei                 # Huawei exploit module
 ┃ ┣ 📂 Joker                  # Advanced Mirai variant
 ┃ ┣ 📂 KryptomeV6             # C2 with telnet loader
 ┃ ┣ 📂 litmus2-bot            # Windows IRC bot
 ┃ ┣ 📂 LoexBot                # Windows bot with spreaders
 ┃ ┣ 📂 meerkat-private        # Private Mirai variant
 ┃ ┣ 📂 MODZ V4.5              # qBot with server/client
 ┃ ┣ 📂 moobotV2               # Moobot variant
 ┃ ┣ 📂 moobotV3               # Moobot variant
 ┃ ┣ 📂 Mortem-qBot            # qBot variant with logging
 ┃ ┣ 📂 Oriation Telnet        # Telnet brute-forcer
 ┃ ┣ 📂 PyRai                  # Python-based Mirai clone
 ┃ ┣ 📂 R.U.D.Y                # RUDY attack implementation
 ┃ ┣ 📂 reaper-sec             # ReaperSec botnet
 ┃ ┣ 📂 Rebirth                # Bot with logging
 ┃ ┣ 📂 Senpai                 # Mirai variant
 ┃ ┣ 📂 THANOS                 # C2 client/server
 ┃ ┣ 📂 toxnet                 # Go-based botnet
 ┃ ┣ 📂 urxbot                 # Windows IRC bot
 ┃ ┣ 📂 xTBot                  # Windows bot
 ┃ ┣ 📂 YBotV2                 # Advanced Mirai variant
 ┃ ┗ 📂 ZBOT                   # Condi-style botnet
 ┗ 📂 ddos/                    # Layer 7 DDoS flooders
   ┣ 📂 browser2               # Browser-based flooder
   ┣ 📂 NoName                 # Browser fingerprint spoofing
   ┣ 📂 NoName2                # Extended browser flooder
   ┣ 📂 oldbrowser             # Legacy browser flooder
   ┗ 📂 Sang-Pematuk           # Multi-method Python flooder
```

---

## 🚀 Getting Started (For Learning)

> **⚠️ REMINDER: These tools are NOT tested and NOT ready for deployment.**

### Prerequisites
- **Linux** (Kali/Ubuntu recommended for most C-based bots)
- **GCC / Make** (for compiling C/C++ bots)
- **Go 1.16+** (for Go-based C2 servers)
- **Python 3.8+** (for Python-based tools)
- **Cross-compilers** (for ARM/MIPS builds — optional)

### 🔬 Suggested Learning Approach

1. **Read the source code** — understand the attack logic
2. **Study the C2 communication** — how bots receive commands
3. **Analyze the exploits** — how routers/IoT devices are targeted
4. **Set up in isolated lab** — NEVER on production networks
5. **Modify and experiment** — tweak parameters, add features
6. **Build detection rules** — create signatures to defend against these

---

## 📚 What You Can Learn

- **Botnet architecture** — C2, loader, bot, DLR, encryption
- **DDoS attack vectors** — UDP, TCP, HTTP, TLS, GRE, ICMP
- **IoT exploitation** — default credentials, web vulnerabilities
- **Malware persistence** — process hiding, anti-debug, self-replication
- **C2 development** — Go/Python/C2 panels with APIs
- **Network scanning** — port scanning, banner grabbing, telnet brute-force
- **Payload delivery** — droppers, downloaders, obfuscation

---

## 🧪 Testing Status

| Category | Status |
|----------|--------|
| **Compilation** | ⚠️ Not fully verified |
| **Functionality** | ⚠️ Not tested |
| **Stability** | ⚠️ Unknown |
| **Security** | ⚠️ May contain backdoors or vulnerabilities |
| **Compatibility** | ⚠️ Legacy code may not run on modern systems |

> **💡 Recommendation:** Treat this as **source code archaeology**. Study the techniques, not for operational use.

---

## ⭐ Final Word

> *"The best way to defend against an attack is to understand how it works."*

This repository is a **digital museum** of offensive security tools. Study them. Learn from them. Use that knowledge to build better defenses.

**Stay ethical. Stay legal. Stay secure.**

---

**🐜 COLONY** — *Preserving the past to protect the future.*

---

## 💀 **WANT MORE?** 💀

> **Looking for private, custom-built botnets with full C2 panels?**  
> Whether you need **Command-Line**, **Graphical**, or **Panel-connected** botnets —  
> **We've got you covered.**

<br>

### 🛠️ **Available Options:**

| Type | Description |
|------|-------------|
| **💻 Command-Line Botnets** | Lightweight, fast, scriptable — for advanced users |
| **🖥️ Graphical C2 Panels** | Full GUI dashboards with real-time monitoring |
| **📡 Panel-Connected Bots** | Fully integrated with web-based management |
| **⚡ Custom Development** | Tailor-made botnets for your specific needs |

<br>

### 📲 **Contact:**

<h3>
  <a href="https://t.me/batmanpriv">
    <img src="https://img.shields.io/badge/Telegram-@batmanpriv-26A5E4?style=for-the-badge&logo=telegram&logoColor=white" alt="Telegram">
  </a>
</h3>

**🔒 Secure. Private. Professional.**

</div>

<hr>

<div align="center">

**🐜 COLONY** — *"Preserving the past to protect the future."*

**Maintainer:** [@batmanpriv](https://github.com/batmanpriv)

</div>
