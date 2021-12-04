
# Sequence Pair based Floorplan with Simulated Annealing optimization

## build
$ make
or open DEBUG mode
$ make DEBUG=DEBUG

## execute
$ ./Lab3.exe <alpha> <input.block name> <input.net name> <output name>

alpha : 0~1 
cost function : alpha* Area + (1-alpha)*hpwl
example 
'''
$ ./Lab3.exe 0.5 ami33/ami33.block ami33/ami33.nets Layout
'''

you can set the <output name> to "Layout"	
then type 
$python .\drawLayout.py to see the result.

