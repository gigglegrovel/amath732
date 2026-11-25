# amath732
# 2D Ising Model – Monte Carlo Simulation

A C implementation of the two-dimensional Ising model using the Metropolis Monte Carlo algorithm with periodic boundary conditions.

The simulation generates an MP4 animation of the spin configurations by streaming lattice data directly to FFmpeg, avoiding large intermediate configuration files.

## Requirements

- GCC compiler
- C standard library
- FFmpeg with the `libx264` encoder

## Compilation

```bash
gcc -O3 -std=gnu11 ising2d.c -o ising -lm
```

## Input parameters

Simulation parameters are read from a file named `input`, located in the working directory.

Example:

```text
equilibration_sweeps 1000
simulation_sweeps 5000
sampling_freq 1
J 1.0
h 0.0
T 2.269
```

| Parameter | Description |
|---|---|
| `equilibration_sweeps` | Number of Monte Carlo sweeps before sampling begins |
| `simulation_sweeps` | Number of Monte Carlo sweeps during sampling |
| `sampling_freq` | Number of sweeps between saved frames |
| `J` | Nearest-neighbor coupling constant |
| `h` | External magnetic field |
| `T` | Temperature in units where k_B = 1 |

Parameters must appear in the specified order.

The lattice dimensions are defined by `lx` and `ly` in the source code. The default lattice size is 1000 × 1000.

## Execution

```bash
./ising
```

## Simulation methodology

- Two-dimensional square lattice.
- Spin variables taking values of +1 or −1.
- Nearest-neighbor interactions.
- Periodic boundary conditions.
- Random single-site selection.
- Metropolis acceptance criterion.
- One Monte Carlo sweep consists of N = lx × ly attempted spin flips.

For the ferromagnetic square-lattice Ising model at zero external field, the exact critical temperature is approximately T_c = 2.269185 J, with k_B = 1.

## Output

The simulation generates:

```text
ising_movie.mp4
```

The animation displays positive spins in white and negative spins in black, at 30 frames per second.

Configurations are streamed directly to FFmpeg, eliminating the need to store raw lattice configurations on disk.

## Repository structure

```text
.
├── ising2d.c
├── input
└── README.md
```

## Performance considerations

Computational cost scales with the lattice size and the number of Monte Carlo sweeps. Increasing the sampling frequency interval reduces video encoding overhead and output size.

Near the critical temperature, equilibration may require significantly more sweeps due to critical slowing down.
