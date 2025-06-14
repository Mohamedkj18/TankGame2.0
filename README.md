# Advanced Topics in Programming - Tank Game (Assignment 2)

## Contributors

- **Mohamed Jabali** - ID: 212788293
- **Waed Iraqi** - ID: 212497127

## Overview

This project is a tank battle simulation implemented in modern C++20 as part of Assignment 2 for the _Advanced Topics in Programming_ course at Tel Aviv University. The game simulates combat between two players, each controlling tanks on a grid-based battlefield.

Each tank follows a deterministic AI strategy and may perform actions such as moving, rotating, shooting shells, or requesting battlefield info. The game ends with a win, tie by destruction, or tie by shell exhaustion over 40 steps.

## Compilation Instructions

To compile the project:

```bash
make
```

Make sure your compiler supports **g++ 11.4 or higher**, and uses the following flags:

```bash
-std=c++20 -Wall -Wextra -Werror -pedantic
```

## Running the Game

Run the executable with:

```bash
./tanks_game <input_file>
```

The game will generate:

- `output_<input_file>.txt` – detailing all tank actions and final result
- `visualization.txt` – a round-by-round snapshot of the battlefield

### Example

```bash
./tanks_game input_a.txt
```

Will produce `output_input_a.txt`.

## File Structure

- `include/` – Header files (Tank, Shell, GameManager, Algorithms)
- `src/` – Source code implementing all game logic
- `main.cpp` – Entry point
- `Makefile` – Build script
- `data/` – Input and output test files

## Input Format

Plaintext file structured as:

```
<Map Description>
MaxSteps = 5000
NumShells = 50
Rows = 10
Cols = 20
<map grid with symbols>
```

### Symbols

- `#` – Wall
- `/` - Damaged Wall
- `@` – Mine
- `1` or digits – Player 1 tanks
- `2` or digits – Player 2 tanks
- Space – Empty corridor

All tanks start facing Left (Player 1) or Right (Player 2).

## Output Format

- One line per game step with comma-separated actions of each tank.
- Action names follow `ActionRequest` enum (`MoveForward`, `Shoot`, etc.)
- Killed tanks are marked as `(killed)`
- Illegal actions are marked as `(ignored)`
- Final line announces the winner or tie reason

## Libraries

Only standard C++ libraries were used. No external libraries.

## Notes

- No manual memory management (`new`/`delete`) is used.
- Smart pointers and STL containers are used for safety and clarity.
- Game logic is modular and object-oriented.

## Bonus

See `bonus.txt` for details on any implemented bonus features.

## Visualization Simulator Usage

To run the visual simulator, use the following command from the project root directory:

```
python visual_simulator/main.py <output_file>
```

- The simulator always loads the visualization file from `data/visualization.txt`.
- `<output_file>` should be the path to the output file you want to visualize (e.g., `data/output_input_a.txt`).

**Example:**

```
python visual_simulator/main.py data/output_input_a.txt
```

**Note:**  
Make sure to run the command from the project root directory so that the simulator can find the required assets in `visual_simulator/assets/`.
