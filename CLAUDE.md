# c-game

A learning project. The long-term goal is a from-scratch 2D game engine in C
(eventually with its own scripting language); see `game-engine-roadmap.md`.
The immediate goal is a small, complete **terminal Snake** game in plain C.

## How this project is worked on

- **The user writes all the code.** This is a teaching setup — the assistant
  guides via small sub-tasks and never writes the user's solution. Do not paste
  solution code into the user's files.
- **From scratch, minimal dependencies.** No third-party libraries or frameworks
  (no SDL, etc.). libc and thin OS interfaces (e.g. `termios` for raw terminal
  input, raw syscalls) are acceptable — they're the OS boundary, not frameworks.
- **No AI attribution in git commits** (no `Co-Authored-By` trailer). Reason:
  since the user writes all the code and the assistant only guides, crediting the
  AI as a (co-)author would be false. The commits are the user's work.
- Git identity: `SjjThaler <stefanthaler@protonmail.com>`.

## Build & run

Plain compile:

    gcc snake.c -o snake && ./snake

Add `-lm` when linking against the math library (e.g. `sqrtf` from `<math.h>`):

    gcc struct.c -o struct -lm && ./struct

Compiled binaries are git-ignored (see `.gitignore`); add new binary names there.

## Layout

- `snake.c` — the current game (in progress).
- `struct.c` — a coordinate-system study artifact (`Point`, `add`, `distance`,
  `move`, `print_point`); kept for reference.
- `game-engine-roadmap.md` — the (preliminary) long-term roadmap.
