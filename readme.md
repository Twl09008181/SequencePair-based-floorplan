
# Sequence Pair based Floorplan with Simulated Annealing optimization	

## Intro  
This is an implementation of fixed-outline floorplan based on Sequence pair  and SA.      

## build	
$ make	

or open DEBUG mode	
$ make DEBUG=DEBUG	

## execute	
$ ./Lab3.exe <alpha> <input.block name> <input.net name> <output name>		

alpha : 0~1 			
cost function : alpha* Area + (1-alpha)*hpwl			
example 			
```
$ ./Lab3.exe 0.5 ami33/ami33.block ami33/ami33.nets Layout	
```


you can set the <output name> to "Layout"			
then type 		
$python .\drawLayout.py to see the result.			

 ami33          
![image](https://user-images.githubusercontent.com/52790122/144703574-3b09ac84-8e82-435d-a0a4-cdab23ecdd4b.png)     
 
ami49         
![image](https://user-images.githubusercontent.com/52790122/144703610-c9b15a13-734a-4970-87b0-64f1016aefdd.png)     

BlockId above is based on input order of each block.
