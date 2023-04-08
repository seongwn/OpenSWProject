import matplotlib.pyplot as plt

def getPlot(dir):
    x_axis = [i for i in range(256)]
    y_axis = []
    file = open(dir, 'r')
    while True:
        line = file.readline()
        if not line:
            break
        _, y = map(float, line.split())
        y_axis.append(y)

    file.close()

    plt.plot(x_axis, y_axis)
    plt.title(dir[:-4])
    #plt.legend()
    plt.show()


dir = "ref_PDF.txt"
getPlot(dir)