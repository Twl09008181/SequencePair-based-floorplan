from matplotlib.lines import Line2D
import matplotlib.pyplot


import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle





def drawLayout(outline,width,height,x_pos,y_pos):
    #define Matplotlib figure and axis
    fig, ax = plt.subplots()

    max_y = 0
    max_x = 0
    for i in range(len(width)):
        ax.add_patch(Rectangle((x_pos[i],y_pos[i]),width[i],height[i],edgecolor='pink',fill=True))
        ax.text(x_pos[i]+width[i]/2,y_pos[i]+height[i]/2,i)
        if(x_pos[i]+width[i] > max_x):
            max_x = x_pos[i] + width[i]
        if(y_pos[i]+width[i] > max_y):
            max_y = y_pos[i] + width[i]

    ax.set_xlim(0,max(max_x,max_y))
    ax.set_ylim(0,max(max_x,max_y))
    outx,outy = outline
    ax.add_line(Line2D([outx,outx],[0,outy],linewidth=5,color="red"))
    ax.add_line(Line2D([0,outx],[outy,outy],linewidth=5,color="red"))
    ax.plot()

    #display plot
    plt.show()



layout = open("Layout","r")
outline_x,outline_y = layout.readline().split()
outline_x = int(outline_x)
outline_y = int(outline_y)
blockNum = int(layout.readline())
print("outline :",outline_x," ",outline_y)
print("blockNum: ",blockNum)

width = []
height = []
x_pos = [] 
y_pos = []

for i in range(blockNum):
    x,y,w,h = layout.readline().split()   
    width.append(int(w))
    height.append(int(h))
    x_pos.append(int(x))
    y_pos.append(int(y))
layout.close()


print(x_pos)
print(y_pos)
print(width)
print(height)

drawLayout((outline_x,outline_y),width,height,x_pos,y_pos)




