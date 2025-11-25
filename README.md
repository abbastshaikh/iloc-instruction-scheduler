This program provides an implementation of an ILOC instruction scheduler.

To build this program, run `make build`. This will generate an executable, schedule, which can be used to run the ILOC instruction scheduler.

There are 2 modes supported:
- `-h`: Prints a help menu.
- `<name>`: Scans, parses, and renames the input ILOC block in `<name>`, then rearranges the instructions in the input block to reduce the number of cycles required to execute the output block.
