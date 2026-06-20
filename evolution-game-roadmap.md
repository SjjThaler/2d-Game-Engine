# Evolution Game Roadmap (C)

A phased plan for a small **two-player game whose opponent is a neural network
improved by evolution** (a genetic algorithm), trained by **self-play**. Built
from scratch in C, no frameworks. This is a concrete first game to *finish*, and
a deliberate exercise of several engine pillars.

> **This plan is preliminary**, exactly like `game-engine-roadmap.md`. It is a
> starting sketch, not a specification. Expect phase order, scope, and the very
> existence of phases to change once real code exists. Every milestone below is a
> hypothesis to revise, not a commitment.

## Relationship to `game-engine-roadmap.md`

This does **not** replace the engine roadmap; it sits alongside it.

- `game-engine-roadmap.md` plans the long arc: *extract a 2D engine from a
  finished game*, eventually with a scripting language.
- This document plans *one specific game* — small, finished, and fun enough to
  motivate the work. Its pieces feed the engine roadmap directly (see the map at
  the end): the fixed-timestep loop, terminal I/O, math, and **above all
  concurrency** (self-play evolution is the project's most natural first reason to
  use multiple cores). The NN-controller-driving-a-player also foreshadows the
  entity/scripting phases.

Keep finishing one small game as the priority. This is a candidate for that game.

## The core concept (one paragraph)

A population of small neural networks plays a simple game against each other.
After each generation, the highest-scoring networks are kept and slightly mutated;
over many generations, play improves — with **no human teaching and no training
data**, only *score → select → mutate*. A human plays the current champion to
*watch* progress, not to drive the learning. A later, deliberately deferred phase
explores adapting the opponent to a specific human's style.

## Guiding principles (read before every phase)

1. **Prove the loop on the smallest thing first.** The one risky question is
   *"does evolution actually produce improvement?"* Answer it earliest, with the
   least scaffolding — before any rendering or human play.
2. **Headless and deterministic before pretty.** Training runs with **no screen
   and no real-time pacing** — a tick is a logical step, not 1/60 of a second.
   Same seed must produce the same match. Rendering and human play are
   *presentation*, bolted on only after the machinery works.
3. **Verify against a fixed target before self-play.** Self-play fitness is a
   *moving* target — everyone improves together, so "is it getting better?" is
   hard to read. First prove fitness climbs against a **fixed scripted opponent**
   (unambiguous signal); *then* switch to self-play for open-ended skill.
4. **Fitness must be graded, not win/lose.** A pure win/lose score gives a
   generation-0 population (all hopeless) nothing to select on. Score by *degree*
   (damage dealt, distance, time survived) so a slightly-less-bad network beats a
   worse one.
5. **From scratch, minimal deps** (project rule). No ML libraries, no NPU / vendor
   SDKs — the neural net is hand-written float arrays. libc + thin OS interfaces
   only.
6. **Defer human-modeling.** The "adapts to *me*" dream is real but hard (see
   Phase G). It is the last phase, not the first.
7. **Performance work waits for a profiler** (project rule). Clear code first.

## Open decisions (decide as you reach them, not before)

- **Which game — DECIDED: pursuit-evasion ("catch me if you can").** Two dots on
  a bounded grid (reuses `snake.c` rendering): the **player chases**, the **NN
  evades**. State is ~4 numbers (pursuer x/y, evader x/y); the action is 2D
  movement; physics is trivial grid stepping. Chosen over Pong because its natural
  fitness — **time survived until caught** — is *dense by construction* (every
  network gets a meaningfully different score), so it sidesteps Pong's sparse
  "count the hits" trap for free (principle 4). It is also two-player: the human
  plays the pursuer (Phase E), and self-play co-evolves pursuer + evader (Phase D).
  Pong and PvP tower-defense remain rejected / a later destination.
  - *Fixed baseline (Phase C):* a greedy chaser that steps toward the evader. It's
    deterministic, so with fixed start positions the evaluation has **zero
    variance** — same network → same survival time.
  - *The real risk is the skill gradient, not variance.* Tune the arena and speeds
    so escape is **hard but possible**: a *bounded arena with corners* (the
    pursuer can trap a careless evader) and roughly equal speed. Too open / equal
    speed → everyone escapes forever (fitness caps out); pursuer too fast →
    everyone caught instantly (fitness floors). Either extreme kills the gradient.
- **State representation** — which numbers feed the NN as input.
- **Fitness function** — the graded score (principle 4).
- **Network shape** — inputs, one small hidden layer to start, outputs.

---

## Phase A — Headless, deterministic game

**Goal:** the chosen game runs with no screen, driven by two *controllers*, and
returns a score. A controller is just "given the game state, produce an action."

Milestones:
1. **Fixed logical timestep** — the update loop advances by ticks, with no
   `sleep` and no wall-clock pacing.
2. **The controller boundary** — the game asks a controller for each side's action
   each tick. Start with hardcoded / random controllers.
3. **Deterministic replay** — same seed and same controllers → identical match and
   identical score, every run.

**Ships when:** two scripted/random controllers play a full match headless and it
prints a final score, reproducibly.

*Covers:* fixed timestep, game state, the state→action interface.

---

## Phase B — The neural-network controller

**Goal:** a network that can stand in as a controller.

Milestones:
1. **Genome = a flat float array** — the network's weights, one contiguous buffer.
2. **Forward pass** — turn the game's state-vector into an action via
   multiply-accumulate over the weights (one small hidden layer to start).
3. **Plug it in** — a random-weight network plays the game (badly) headless.

**Ships when:** a randomly-initialized network plays a full match and produces a
score.

*Covers:* arrays, the forward pass, "the genome is just numbers."

---

## Phase C — The evolutionary loop (the proof of concept)

**Goal:** prove that evolution produces improvement. Evaluate against a **fixed
scripted opponent** so the signal is unambiguous (principle 3).

Milestones:
1. **Population** — many genomes.
2. **Evaluation** — each plays the fixed opponent; record a graded fitness.
3. **Selection + mutation + elitism** — keep the best, copy-and-jiggle to refill,
   carry the top few unchanged so a bad mutation can't erase progress.
4. **Generations** — repeat; print average and best fitness per generation.

**Ships when:** average/best fitness **measurably climbs over generations**. The
concept is proven the moment that line goes up.

*Covers:* genetic algorithms, selection/mutation/elitism, dense-vs-sparse fitness.

---

## Phase D — Self-play

**Goal:** replace the fixed opponent with self-play for open-ended skill.

Milestones:
1. **NN vs NN** — fitness comes from playing other members of the population.
2. **Guard against forgetting** — keep an archive of past champions (a *hall of
   fame*) and test against them, so skill against earlier strategies isn't lost.
3. **Varied starts** — randomize *starting conditions* for robustness, then let
   real game dynamics play each match forward (never grade isolated snapshots).

**Ships when:** later champions reliably beat earlier champions — a visible arms
race.

*Covers:* co-evolution, catastrophic forgetting, fitness aggregation.

---

## Phase E — Render it and play the champion

**Goal:** see it, and play against it for fun. Pure presentation.

Milestones:
1. **Terminal rendering** of a match.
2. **Real-time loop** — now *with* wall-clock pacing (contrast Phase A's headless
   loop) and raw-mode keyboard input (reuse the `snake.c` experience).
3. **Human vs champion** — play a full round against the current best network.

**Ships when:** a human can play a full match against the champion in the terminal.

*Covers:* terminal rendering, raw input, real-time vs logical time.

---

## Phase F — Speed: headless tuning, then parallelism

**Goal:** make training fast — *only* once a profiler says you need it.

Milestones:
1. **Free wins** — no I/O during training, `-O2`, early-terminate decided matches.
2. **Parallelism** — every match in a generation is independent (no shared
   changing data), so run them across CPU cores. Each match needs its **own**
   scratch state and its **own** RNG. Breed only at a **barrier**: parallel within
   a generation, serial at the seam between generations.

**Ships when:** a generation evaluates measurably faster across cores, with
identical results to the serial version.

*Covers:* profiling, embarrassingly-parallel workloads, data races, barriers.
(This is the natural entry point to the engine roadmap's Phase 6.)

---

## Phase G — Adapt to the human (deferred / stretch)

**Goal:** the original dream — an opponent that adapts to a *specific* human.

The hard parts, recorded now so we don't forget the reasoning:
- **Model the human** from recorded `(state, action)` pairs — start instance-based
  (nearest-neighbor: "what did I do in the most similar past situation?").
- **The open-loop replay trap** — a recorded *trajectory* (replay by clock)
  desyncs the moment the opponent behaves differently; replay by *state* (a
  policy) instead, so the model reacts.
- **Robustness vs. sharpness** — evolving only against the latest behavior causes
  *catastrophic forgetting*; keep a collection of behaviors and choose how to
  aggregate (average = sharp but has blind spots; worst-case = robust but
  generic).

**Ships when:** the opponent demonstrably shifts its play in response to a change
in the human's style.

*Covers:* opponent modeling, behavioral cloning, on/off-distribution data.

---

## How this feeds `game-engine-roadmap.md`

| This game's phase            | Engine roadmap it exercises / feeds            |
|------------------------------|------------------------------------------------|
| A (fixed timestep, game state) | Phase 1 (game loop, state), Phase 2 (extract) |
| B (math of the forward pass) | Phase 1 (vector math), Phase 2 (math module)   |
| E (rendering, raw input)     | Phase 1 (rendering, input), Phase 2 (platform) |
| F (parallel self-play)       | Phase 6 (job system / concurrency)             |
| B/D (a network drives a player) | Phase 3–4 (entities, script-driven behavior) |

## Honestly out of scope / known risks

- **The NPU is unusable here.** "AI"-branded CPUs accelerate ML via vendor
  frameworks; from-scratch C nets run on the regular cores. Hardware is not the
  bottleneck.
- **A too-simple game gets *solved*.** With a tiny state and few actions,
  evolution may converge to near-optimal play quickly and then stop *feeling* like
  it adapts. Depth of the game, not compute, is the real risk to the fun.
- **Phase G is genuinely hard** and may stay a stretch goal indefinitely.
