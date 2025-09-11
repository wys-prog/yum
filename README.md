# YumEngine — Lua & Godot Runtime

**Current Engine Version:** 1.6.xx-stable <br>
**[Website](https://wys-prog.github.io/yum)**

---

## Overview

YumEngine is a lightweight, cross-language runtime and engine bridging **C++ / C** with **Lua** and **Godot**.  
It is designed for easy integration, extensibility, and rapid game development or scripting automation.

Key goals:  
- Minimal overhead  
- Maximum flexibility  
- Cross-language interoperability  

---

## Features

- **Dynamic Library (Yum)**: Provides core utilities accessible from C, C++, Lua, and C#.  
- **YumEngine**: Full engine with linked Yum library, including API and internal classes.  
- **YumiEngine**: Engine only, depends on Yum library.  
- **Variant & Vector**: Type-safe, C-compatible containers for multi-language data exchange.  
- **Cross-Language Callbacks**: Register functions callable from Lua, C#, or other languages.  

---

## Requirements

- C++20 compiler (or higher, e.g., `g++-15` on macOS)  
- C17 compatible compiler  
- Optional: Lua interpreter to run scripts  

---

## Installation & Building

Official builds are precompiled. You usually **don’t need to compile** anything manually.  

To build manually:

```bash
lua build.lua build [patch|minor|major]
````

**Notes:**

* Adjust compiler paths or flags in `build.lua` if needed.
* Version bumping is handled automatically via `build.lua version [patch|minor|major]`.
* A `version.js` file is generated for website integration.

---

## Downloads

Get the latest builds here:

[Download YumEngine](https://github.com/wys-prog/yum)

For source code and contributions, see the GitHub repository.

---

## Studio

**Yum Studio** is the team behind YumEngine:

* Studio: `yum-official`
* Branch: `stable`

We are a small, experimental studio passionate about cross-language runtime engines and anime-inspired development tools.

---

## Contributors

* **Wys** — Lead Developer ([GitHub](https://github.com/wys-prog))

---

## License

[License](./LICENSE)

---

> ⚠️ YumEngine is currently under active development. Some features may change or be incomplete. Use at your own risk.
