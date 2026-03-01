So I always have this failing test for `ls | sort | head -5` (although when I manually run it it seems to be correct). When I inspect the tesh_dsh3.py, it seems like the clean_output function would basically skips anything that starts with dsh, and this folder has exactly 5 dsh files: 

```shell
dsh
dsh.dSYM\
dsh_cli.c
dshlib.c
dshlib.h
```
and that when I didn't take that into account for the output, the actual number of files shown would be 0 (different from the assertion of nonzero). So I just pad with this file to see if my code works!