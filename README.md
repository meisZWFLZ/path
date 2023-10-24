# LemLib Path File Format Encoder/Decoder

## Usage

```
cmake --build build; ./build/src/main_program
```

## Development

Install Catch2
https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md#installing-catch2-from-git-repository

Compile
```
cmake -Bbuild
cmake --build build
cmake --build build && ./build/test/tests
cmake --build build && ./build/test/tests --benchmark-samples 1000
cmake --build build && ./build/test/tests --durations yes
export CFLAGS="-m32"; cmake --build build && valgrind --leak-check=yes ./build/test/tests
```

## Format

### Path File

```markdown
 0                   1                   2                   3  
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
┌───────────────┬───────────────────────────────────────────────┐
│    Size {M}   │                                               │
├───────────────┘                                               │
│                                                               │
│               Metadata with Length {M} Bytes ...              │
│                                                               │
├───────────────────────────────┬───────────────────────────────┤
│         Path Count {P}        │                               │
├───────────────────────────────┘                               │
│                                                               │
│                                                               │
│                                                               │
│                                                               │
│                                                               │
│                                                               │
│                         {P} Paths ...                         │
│                                                               │
│                                                               │
│                                                               │
│                                                               │
│                                                               │
│                                                               │
├───────────────────────────────────────────────────────────────┤
│                                                               │
│                                                               │
│                                                               │
│                                                               │
│                        Editor Data ...                        │
│                                                               │
│                                                               │
│                                                               │
│                                                               │
└───────────────────────────────────────────────────────────────┘
```

### For Each Path

```markdown
 0                   1                   2                   3  
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
┌───────────────────────────────────────────────────────────────┐
│      Path Name in N-Bits Null Terminated Ascii String ...     │
├───────────────┬───────────────────────────────────────────────┤
│    Size {M}   │                                               │
├───────────────┘                                               │
│                                                               │
│               Metadata with Length {M} Bytes ...              │
│                                                               │
├───────────────────────────────────────────────────────────────┤
│                       Waypoint Count {W}                      │
├───────────────────────────────────────────────────────────────┤
│                                                               │
│                                                               │
│                                                               │
│                                                               │
│                                                               │
│                                                               │
│                       {W} Waypoints ...                       │
│                                                               │
│                                                               │
│                                                               │
│                                                               │
│                                                               │
│                                                               │
└───────────────────────────────────────────────────────────────┘
```

### For Each Waypoint

The decoder first read 32 bits for the position of the waypoint. Then it reads 1 bytes for the flags. It reads or skips the parameters "heading", "lookahead" and other unknown parameters according to the corresponding flag bits in a fixed order.

#### Example 1: The flag bits are 0b00000000. The decoder reads 32 bits for the position, 16 bits for the speed and skips the rest.

```markdown
 0                   1                   2                   3                  
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
┌───────────────┬───────────────────────────────┬───────────────────────────────┐
│   0b00000000  │           X Position          │           Y Position          │
├───────────────┴───────────────┬───────────────┴───────────────────────────────┘
│             Speed             │                                                
└───────────────────────────────┘                                                 
```

#### Example 2: The flag bits are 0b00000001. The decoder reads 32 bits for the position, 16 bits for the speed and 16 bits for the heading.

```markdown
 0                   1                   2                   3                  
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
┌───────────────┬───────────────────────────────┬───────────────────────────────┐
│   0b00000001  │           X Position          │           Y Position          │
├───────────────┴───────────────┬───────────────┴───────────────┬───────────────┘
│             Speed             │            Heading            │                
└───────────────────────────────┴───────────────────────────────┘                
```

#### Example 3: The flag bits are 0b00000010. The decoder reads 32 bits for the position, 16 bits for the speed, skips heading, and reads 16 bits for the lookahead.

```markdown
 0                   1                   2                   3                  
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
┌───────────────┬───────────────────────────────┬───────────────────────────────┐
│   0b00000010  │           X Position          │           Y Position          │
├───────────────┴───────────────┬───────────────┴───────────────┬───────────────┘
│             Speed             │           Lookahead           │                
└───────────────────────────────┴───────────────────────────────┘                
```

#### Example 4: The flag bits are 0b00000100. The decoder reads 32 bits for the position, 16 bits for the speed, skips heading and lookahead, and reads 16 bits for the unknown parameter 1.

```markdown
 0                   1                   2                   3                  
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
┌───────────────┬───────────────────────────────┬───────────────────────────────┐
│   0b00000100  │           X Position          │           Y Position          │
├───────────────┴───────────────┬───────────────┴───────────────┬───────────────┘
│             Speed             │      Unknown Parameter 1      │                
└───────────────────────────────┴───────────────────────────────┘                
```

#### Example 5:

```markdown
 0                   1                   2                   3                  
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
┌───────────────┬───────────────────────────────┬───────────────────────────────┐
│   0b10000110  │           X Position          │           Y Position          │
├───────────────┴───────────────┬───────────────┴───────────────┬───────────────┤
│             Speed             │           Lookahead           │Unknown Paramet│
├───────────────┬───────────────┴───────────────┬───────────────┴───────────────┘
│               │      Unknown Parameter 6      │                                
└───────────────┴───────────────────────────────┘                                
```

### Details

```markdown
All-Integer:  In Little-Endian
Size:         Unsigned Integer, can be 0
Count:        Unsigned Integer, can be 0
Path Name:    Null-Terminated ASCII string
Speed:        Signed Integer, mm/s/bit, range: -32768mm/s ~ +32767mm/s
Y Position:   Signed Integer, 0.5mm/bit, range: -16384mm ~ +16383.5mm
Y Position:   Signed Integer, 0.5mm/bit, range: -16384mm ~ +16383.5mm
Heading:      Unsigned Integer , 0.0001rad/bit, range: 0rad ~ 6.2832rad
Lookahead:    Signed Integer, 0.5mm/bit, range: -16384mm ~ +16383.5mm
```
