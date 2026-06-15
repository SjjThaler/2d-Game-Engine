# Game Engine Roadmap (C)

A phased plan for building a 2D game engine in C, starting from a small finished
game. Designed to be handed to a planning assistant that breaks each phase into
concrete steps, while you write all the code.

> **This plan is totally preliminary.** It's a starting sketch, not a
> specification. Phase order, scope, tech choices, and the existence of whole
> phases are all expected to change once real code exists and you learn what the
> project actually wants to be. Treat every milestone below as a hypothesis to
> revise, not a commitment to honor.

## How to use this document

- Work **one phase at a time**, and within a phase, **one milestone at a time**.
- Ask the planner to expand only the *current* milestone into steps. Don't let it
  (or yourself) plan three phases ahead — the design will change once code exists.
- Every milestone ends in a **program that compiles and runs**. If a milestone
  can't ship something runnable, it's scoped wrong — split it.

## Guiding principles (read before every phase)

1. **Finish the game before you have an engine.** Phase 1 is a real, playable,
   monolithic game with no abstractions. This is the whole point — it proves you
   finish things and gives you concrete code to *extract* an engine from.
2. **Extract, don't pre-build.** The engine is what's left after you generalize
   working game code. Never build an abstraction for a game that doesn't exist yet.
   This is the #1 way engine projects die.
3. **Keep the scripting language minimal.** The smallest language that can express
   game logic and call engine functions. No fancy type system, no module system,
   until a script is actually driving an entity.
4. **Performance work waits for a profiler.** Write clear code first. Optimize the
   memory layout only when you've measured a real bottleneck.

## Tech decisions (decide once, up front)

- **Language:** C11.
- **2D, not 3D.** 3D multiplies the math, rendering, and asset complexity and is a
  common reason these projects stall. 2D still exercises every pillar.
- **Platform/graphics library:** SDL2 (windowing, input, 2D rendering, audio,
  timing — all cross-platform). Avoid writing a platform layer from scratch first;
  you can replace SDL later if you want that exercise.
- **Build system:** a Makefile to start; migrate to CMake only if cross-platform
  builds become a real need.
- **Version control:** git from commit one.

---

## Phase 0 — Foundations

**Goal:** a window you can close.

- Toolchain, build system, git repo, directory layout.
- Link SDL2. Open a window, clear it to a color, handle the quit event, run a basic
  loop at a fixed frame rate.

**Ships when:** a window opens, shows a solid color, and closes cleanly.

*Covers:* C build tooling, the program lifecycle.

---

## Phase 1 — A small, complete game (monolithic, no abstractions)

**Goal:** one finished, playable, slightly fun game. This is the most important
phase. Do not skip it, do not generalize anything in it.

Pick **one** small-but-complete game — Breakout, Asteroids, Snake, or Pong.
Breakout or Asteroids are good: enough vectors and collision to be interesting,
small enough to finish.

Milestones:
1. **Game loop with a fixed timestep** — decouple the update rate from the render
   rate (accumulator pattern). Understand why this matters.
2. **Input** — read keyboard/mouse, map to game actions.
3. **Rendering** — draw shapes/sprites for the player and objects.
4. **Vector math** — a tiny `vec2` with add/scale/dot/length, inline in the game.
5. **Collision** — AABB or circle checks; basic response.
6. **Game state** — menu / playing / game-over, score, win and lose conditions.
7. **Audio** — a couple of sound effects.

**Ships when:** someone can launch it, play a full round, lose or win, and play
again — and it's a little bit fun.

*Covers:* C and pointers, real-time loop timing, applied algorithms (collision),
math, basic audio. (Reinforces CS:APP-style thinking; no engine yet.)

---

## Phase 2 — Extract the engine

**Goal:** the same game, now running on top of engine modules you pulled out of it.
The game should play *identically* — this phase changes structure, not behavior.

Milestones:
1. **Draw the engine/game line.** List what in Phase 1 is reusable (window, input,
   rendering, timing, audio, math) vs game-specific (rules, levels, scoring).
2. **Platform module** — window, input, time behind a clean API.
3. **Renderer module** — a small drawing API the game calls.
4. **Audio module.**
5. **Math library** — promote the inline vec2 to a real header.
6. **Memory module** — introduce an arena/bump allocator and a frame allocator.
   This is your deliberate CS:APP intersection: control allocation explicitly
   instead of scattering `malloc`/`free`.

**Ships when:** the Phase 1 game runs unchanged through the new engine API, and the
game code no longer calls SDL directly.

*Covers:* software architecture, manual memory management, the memory hierarchy
(data layout, allocators).

---

## Phase 3 — Entity system

**Goal:** replace hardcoded game objects with a general entity representation that
scripts can later address by handle.

Milestones:
1. **Entities as IDs + components** — start simple: entities are integer IDs,
   components live in arrays indexed by ID. Resist a full generic ECS on day one.
2. **A handle scheme** — IDs that can be validated (generation counter) so a stale
   reference is detectable rather than a dangling pointer.
3. **Port the game** onto entities/components.
4. *(Optional, later)* move toward contiguous component arrays for cache locality —
   only after profiling.

**Ships when:** the game runs entirely on the entity system and you can spawn/
destroy entities by handle.

*Covers:* data structures, data-oriented design, cache-aware layout (CS:APP).

---

## Phase 4 — The scripting language (the compiler / PL pillar)

**Goal:** a small interpreter, written in C, that drives game logic and calls
engine functions. Blueprint: the "clox" bytecode VM from *Crafting Interpreters*
(a tree-walking interpreter is an acceptable first version).

Milestones:
1. **Lexer** — source text to tokens.
2. **Parser** — tokens to an AST. Variables, functions, control flow, arithmetic.
3. **Interpreter** — evaluate the AST (or compile to bytecode and run a VM).
4. **Values + garbage collector** — a tagged value type; a mark-sweep GC. This is
   memory management from the other side — pairs directly with Phase 2.
5. **Native function binding** — register C functions callable from script.
6. **Handle marshalling** — pass entity handles (never raw pointers) across the
   boundary; validate them on the C side.
7. **Engine integration** — expose `spawn`, `move`, `get_position`, `play_sound`;
   have the engine call the script's `update(dt)` and event hooks each frame.
8. **Hot reload** — reload a script while the game runs without losing state.
9. **Port some game logic** from C into scripts to prove the loop end to end.

**Ships when:** an enemy (or some real behavior) is controlled entirely by a script,
and you can edit that script and see it change live.

*Covers:* compilers/PL (lexing, parsing, interpretation), garbage collection,
the C↔language FFI boundary.

---

## Phase 5 — Tooling & asset pipeline (optional breadth)

**Goal:** the conveniences that make the engine usable, while touching parsing and
storage concepts.

- **Level / data format** — a simple text format with its own small parser (more
  PL practice) or a structured data file.
- **In-game console** — runs your scripting language interactively (nice payoff).
- **Save/load system** — serialization to disk. This is the closest this project
  gets to the database pillar (persistence, layout, versioning) — but it is *not*
  a real database. If storage/queries/transactions genuinely interest you, that's
  a separate project.

**Ships when:** you can load a level from a file and save/restore game state.

*Covers:* parsing/serialization, light persistence concepts.

---

## Phase 6 — Concurrency (advanced, only if the engine needs it)

**Goal:** parallelism where it earns its place.

- **Job system / thread pool** — dispatch independent work (asset loading,
  parts of update) across threads.
- Synchronization, data races, and why immutable/owned data simplifies them.

**Ships when:** a measurable workload runs across multiple threads correctly.

*Covers:* concurrency, synchronization, OS scheduling realities (CS:APP, OSTEP).

---

## Phase 7 — Networking (stretch; a large pillar on its own)

**Goal:** multiplayer for a game built on the engine.

- **Sockets + a simple protocol.**
- **Client–server** with one authoritative server.
- **State synchronization**, then the hard parts: client-side prediction, lag
  compensation, reconciliation.

**Ships when:** two clients see a consistent shared game state over a network.

*Covers:* networking, distributed-state problems, the hard version of concurrency.

---

## Breadth coverage map

| CS area                         | Where it shows up                          |
|---------------------------------|--------------------------------------------|
| C / pointers / memory mgmt      | Phases 1–2, 4 (GC)                          |
| Memory hierarchy / performance  | Phases 2–3 (allocators, data layout)       |
| Data structures & algorithms    | Phases 1, 3 (collision, entities)          |
| Math (linear algebra)           | Phases 1–2                                  |
| Compilers / programming languages | Phase 4 (and 5's parsers)                |
| Garbage collection / memory     | Phase 4                                     |
| Concurrency / OS                | Phase 6                                     |
| Networking                      | Phase 7                                     |
| Graphics                        | Phases 1–2 (2D rendering)                   |

## Honestly out of scope

These won't be meaningfully covered and shouldn't be forced into the engine:

- **Theory of computation** (automata, computability, complexity proofs).
- **Most discrete math** — appears implicitly, not as a focus.
- **Real databases** — storage engines, query optimization, transactions. Phase 5
  only brushes persistence. If this pillar matters to you, build a separate small
  database later; don't bolt it onto the engine.
