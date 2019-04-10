# Building

Windows: Use the Visual Studio Solution that builds the program on a remote Linux server.

Linux: Use `make` to build the program using g++.

# Usage

`./sv -n/--size <size of playing field> -d/--dimension <dimensions> -b/--data-size <data size exponent> -c/--ships-size <ships size exponent> -s/--strategy <strategy name> -t/--threads <amount of threads> -x/--seed <prng seed>`

Available strategies:
- `random`

## Example
`./sv -n 10 -d 8 -t 10 -s random -b 6 -c 7 -t 1 -x 1234`
