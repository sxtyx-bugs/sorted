# SORTED.
### A file organiser. Because apparently, putting files in folders required an engineering degree.

> Built for a 10-mark DAA assignment.
>
> Deployed like we're expecting an acquisition from Apple.

---

## What is this?

SORTED is a desktop file organiser built by **Satydev**, using C++17 and Qt 6.

It scans your workspace, classifies files, calculates an optimal organisational plan, resolves naming conflicts and asks for your permission before moving anything.

In simpler terms, it puts your PDFs where they belong.

Yes. We made an entire application for that.

No, we don't think it was excessive.

---

## The assignment vs. the response

| The professor asked for | We delivered |
|---|---|
| A DAA application | A desktop product |
| Sorting algorithms | An actual planning engine |
| File classification | Classification pipeline |
| Basic interface | Qt 6 desktop application |
| File organisation | Preview, approval and execution |
| Some analysis | Collision resolution and recovery journal |
| A project submission | Cross-platform CI/CD and native installers |

**10 marks. 100% unnecessary commitment.**

---

## Features

- **Workspace Scanning** — Finds eligible files without rummaging through existing subdirectories like an uninvited guest.
- **Intelligent Classification** — Gives files a proper place in life.
- **Organisation Planning** — Calculates where everything should go before touching a single file.
- **Collision Resolution** — No overwriting. We're civilised.
- **Human Approval** — The algorithm makes suggestions. You retain the authority to disagree with a glorified sorting machine.
- **Operation Journal** — Keeps a record of what happened. Unlike your group project members.
- **Undo & Recovery** — Because even excellent engineering occasionally needs an apology.
- **Cross-platform Builds** — Windows and macOS. One operating system simply felt a bit provincial.

---

## Technology Stack

| Technology | Purpose |
|---|---|
| C++17 | Core logic |
| Qt 6 Widgets | Desktop interface |
| CMake | Build system |
| `std::filesystem` | File operations |
| Qt Concurrent | Background processing |
| CTest | Automated testing |
| GitHub Actions | CI/CD and release automation |

We could have written a script.

We chose C++.

We could have stopped there.

We didn't.

---

## Native Application Releases

Because apparently, compiling the application wasn't enough. We had to distribute it too.

| Platform | Distribution |
|---|---|
| Windows x64 | `.exe` installer |
| macOS Apple Silicon | `.dmg` disk image |

Both are packaged application builds, not instructions telling you to clone a repository and spend your evening configuring CMake.

**Download:** [SORTED Releases](https://github.com/sxtyx-bugs/sorted/releases/latest)

---

## How it works

1. **Scan** — Inspect eligible files in the selected workspace.
2. **Classify** — Determine the appropriate category.
3. **Plan** — Calculate destination paths.
4. **Resolve** — Handle collisions without overwriting existing files.
5. **Preview** — Show the proposed arrangement.
6. **Approve** — Wait for the human to authorise the operation.
7. **Execute** — Move the files.
8. **Journal** — Record the operation for traceability and recovery.

Eight steps to accomplish what dragging a file might have achieved.

Efficiency is subjective.

---

## Safety, because we're not monsters

- Existing subdirectories are protected.
- No automatic duplicate deletion.
- No silent file overwrites.
- No file movement without explicit approval.
- Destination paths are revalidated before operations.
- Operations are journalled for recovery.
- The application does not organise its own build output.

Your Downloads folder has suffered enough.

---

## Build locally

Requirements:
- C++17 compiler
- CMake
- Qt 6
- Ninja or a compatible build system

```bash
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

---

## Frequently Asked Questions

**Why not just use Python?**

We wanted a desktop application, not another script that requires three dependencies, two terminals and a prayer.

**Is this really for 10 marks?**

Yes.

**Was all of this necessary?**

Absolutely not.

**Would we do it again?**

Unfortunately, yes.

**Does it organise your life?**

No. We have standards.

**Can it organise your Downloads folder?**

Yes. Your file naming conventions, however, are beyond the scope of this project.

---

## Academic Context

SORTED was developed by **Satydev** as part of a Design and Analysis of Algorithms (DAA) academic project.

It demonstrates algorithmic problem-solving through sorting, classification, planning and collision resolution, with additional emphasis on filesystem safety and practical software engineering.

The assignment carried a modest academic weight.

The implementation carried an entirely different emotional weight.

---

## Final Remarks

Some students submitted an algorithm.

Some submitted a visualisation.

We submitted a desktop application with a build pipeline, native Windows `.exe` installer, macOS `.dmg`, filesystem safeguards, operation journalling and cross-platform ambitions.

All for 10 marks.

**SORTED.**

*Because if you're going to overengineer something, at least organise it properly.*

---

**Created by [Satydev](https://satydev.com)** · [GitHub](https://github.com/sxtyx-bugs)

