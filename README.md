# 🐆 Cheetah (Work-in-Progress)

**Cheetah** (temporary name) is a high-performance automation scripting language, designed to run fast with optional C/C++ acceleration.

---

## What is Cheetah?

- Built for **desktop automation**: generating projects from templates, cleaning files, configuring apps, syncing to cloud, and more.
- Written entirely in **C++**, with the ability to **load dynamic C++ modules** for performance-critical tasks.
- Offers **type safety** and stability through strong typing and rigorous checks.

---

## Key Features

- **Speed-first design** — easily accelerated via native C++ modules.
- Flexible and safe: if a feature is slow or unstable, rewrite that module in C++.
- Focused tooling for everyday desktop automation needs.
- Emphasizes developer control and extensibility.

---

## How It Works

1. **Script-first**: write your automation logic in the Cheetah DSL.
2. **Accelerate as needed**: compile performance-critical bits as C++ modules.
3. **Run locally** — handle tasks like project scaffolding, desktop cleanup, cloud sync, and template-based code generation.
4. **Safe and type-checked**, minimizing runtime errors and unexpected behavior.

---

## Getting Started

```bash
git clone https://github.com/adalspace/cheetah.git
cd cheetah
mkdir build && cd build
cmake ..
make
./cheetah your-script.cht
```

Use the `examples/` folder to explore starter scripts and automation workflows.

---

## Why C++ Modules?

- **Modular extension**: plugins written in C++ can be loaded dynamically.
- **Zero compromise on performance**: rewrite parts that matter most for efficiency.
- **Leverage existing C++ libraries** with ease.

---

---

## Goals & Roadmap

| Milestone      | Progress |
|----------------|----------|
| Core DSL       | ✅ Working |
| C++ module API | 🔧 In progress |
| Macros & Type checks | 🚧 WIP |
| CLI & Formatting | 🚧 To do |
| Documentation & Examples | 📝 On the way |

---


Happy automating!

*— [adalspace](https://github.com/adalspace)*
