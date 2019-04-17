# Building

Windows: Use the Visual Studio Solution that builds the program on a remote Linux server.

Linux: Use `make` to build the program using g++.

# Usage

`./sv -n/--size <size of playing field> -d/--dimension <dimensions> -b/--data-size <data size exponent> -c/--ships-size <ships size exponent> -s/--strategy <strategy name> -t/--threads <amount of threads> -x/--seed <prng seed>`

Available strategies:
- `random`

## Example
`./sv -n 10 -d 8 -t 10 -s random -b 6 -c 7 -x 1234`

# Output

The program outputs a file called `results-<n>-<d>.txt` which contains data for two plots.
The content has the form `<sample size> <population size> <amount of cells> (data_point)+ <expected ship value> <expected fleet value>`.
Each data_point has the format `x,y_1,y_2`.

# TODO

Erstmal könnt ihr folgendes ausführen:
Die results Datei und den Konsolenoutput am Ende des Durchlaufs speichern, die brauchen wir für die Auswertung.

`ca. 20h: ./sv -n 10 -d 15 -b 8 -c 5 -s random -t 128`

`ca. 2h: ./sv -n 10 -d 14 -b 8 -c 5 -s random -t 128`

`ca. 2h: ./sv -n 10 -d 13 -b 8 -c 6 -s random -t 128`

`ca. 2h: ./sv -n 10 -d 12 -b 8 -c 7 -s random -t 128`

`ca. 2h: ./sv -n 10 -d 11 -b 8 -c 7 -s random -t 128`

`ca. 2h: ./sv -n 10 -d 10 -b 8 -c 7 -s random -t 128`

`ca. 2h: ./sv -n 10 -d 9 -b 8 -c 7 -s random -t 128`

`ca. 2h: ./sv -n 10 -d 8 -b 8 -c 7 -s random -t 128`

`ca. 2h: ./sv -n 10 -d 7 -b 7 -c 7 -s random -t 128`

`ca. 2h: ./sv -n 10 -d 6 -b 6 -c 7 -s random -t 128`

`ca. 2h: ./sv -n 10 -d 5 -b 5 -c 7 -s random -t 100`

`ca. 2h: ./sv -n 10 -d 4 -b 4 -c 7 -s random -t 1`

`ca. 2h: ./sv -n 10 -d 3 -b 3 -c 6 -s random -t 1`

`ca. 2h: ./sv -n 10 -d 2 -b 2 -c 4 -s random -t 1`

`ca. 2h: ./sv -n 10 -d 1 -b 1 -c 2 -s random -t 1`
