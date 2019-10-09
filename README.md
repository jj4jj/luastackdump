# luastackdump
register signal to dump running stack



##test##

```
#need make lua5.1 and make local #(install)


make

lua test.lua

traceback stack:
[1][Lua]'test.lua':23 @h
[2][Lua]'test.lua':17 @g
[3][Lua]'test.lua':13 @f
[4][main]'test.lua':30 @--
[5][C]'[C]':0 @--


```
