# BasicAssembler
A very basic assembler for a custom built 4 Bit CPU at collage. Ìt also supports inline functions. 

## Usage
Run the compiler.exe in the therminal with the following parameters:

```compiler.exe <MachineInstructions.inst> <Source file> <Max Machine Code rows> <true/ false if the output is in hex or binary>```

The source code looks something like this:

```
@begin

inline foo
Add
Sub
endf

inline LoadAlu
MovRamReg 11001100
MovRegReg
Sub
MovRegRam 11001100
endf

:foo
:LoadAlu

endp
```

Resulting in an output like this:

```
1011
1111
0000
0000
0001
...
```

## MachineInstr.inst
The file MachineInstr.inst defines the available instructions for the CPU and its defined binary/ hex code. The last parameter defines the amount of parameter the command takes, for example the jmp
takes 2 arguments for a 8 Bit Adress (2x 4 Bit). So remember to split up a 8 Bit adress into 2x 4 Bit adresses.

```
Add 0000 0
Sub 0001 0
Mult 0010 0
And 0011 0
Or 0100 0
Not 0101 0
Reserved 0110 0
Reserved 0111 0
MovRamReg 1000 2
MovRegRam 1001 2
MovRegReg 1010 0
Jmp 1011 2
Jmpc 1100 2
Jmpz 1101 2
Jmpo 1110 2
End 1111 0
```
