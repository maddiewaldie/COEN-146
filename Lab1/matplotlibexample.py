# import the neccessary libraries required for the plotting of a graph, such as matplotlib
import matplotlib.pyplot as plt

#list of the x-values and the y-values we want to plot
y = [0, 3.2, 6, 9, 13, 15.9, 18.333, 23, 24, 27, 30]
x = [0,
     100000000,
     200000000,
     300000000,
     400000000,
     500000000,
     600000000,
     700000000,
     800000000,
     900000000,
     1000000000]

#the plot() function creates the plot
plt.plot(x,y) #make sure that the x and the y list contains the same number of elements

#xlabel() and ylabel() functions helps in renameing the titles of the x and y axis
plt.xlabel('File Byte Size')
plt.ylabel('Time in Seconds')

#include the title of the graph using the title() function 
plt.title('Time to Copy File')

#saves the graph as a file 
plt.savefig("nameofthefile.png")

#shows the output of the graph on the screen
plt.show()
