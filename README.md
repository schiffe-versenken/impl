# Building

Windows: Use the Visual Studio Solution that builds the program on a remote Linux server.

Linux: Use `make` to build the program using g++.

# Usage

`./sv -n/--size <size of playing field> -d/--dimension <dimensions> -b/--data-size <data size exponent> -c/--ships-size <ships count> -s/--strategy <strategy name> -t/--threads <amount of threads> -x/--seed <prng seed>`

Available strategies:
- `random`

## Example
`./sv -n 10 -d 8 -t 10 -s random -b 6 -c 10000 -x 1234`

# Output

The program outputs a file called `results-<n>-<d>.txt` which contains data for two plots.
The content has the form `<sample size> <population size> <amount of cells> (data_point)+ <expected ship value> <expected fleet value>`.
Each data_point has the format `x,y_1,y_2`.

# TODO

Siehe results repository
