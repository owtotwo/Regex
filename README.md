# Regex
---
### RegexEngine ###

The practice of the Compiler course.

It will transfrom the regex expression into a corresponding table.
And in this procedure, it will transfrom into a NFA and then a DFA first.
The form of table is like this:

```
  StartState 0
  EndState 2
  Table
       a  b  c  d ...
   0 | 1 -1  1  0 ...
   1 | 2 -1  2  1 ...
   2*| 0  1 -1 -1 ...
```
