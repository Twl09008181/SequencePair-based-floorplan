import matplotlib.pyplot


import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle





def drawLayout(width,height,x_pos,y_pos):
    #define Matplotlib figure and axis
    fig, ax = plt.subplots()

    for i in range(len(width)):
        ax.add_patch(Rectangle((x_pos[i],y_pos[i]),width[i],height[i],edgecolor='pink',fill=True))
        ax.text(x_pos[i]+width[i]/2,y_pos[i]+height[i]/2,i)

    ax.plot()

    #display plot
    plt.show()



layout = open("Layout","r")
outline_x,outline_y = layout.readline().split()
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

drawLayout(width,height,x_pos,y_pos)



