import numpy as np

def array_init(arr):
    """
    helper function to format an arbitrary shaped array into the content of
    a C++ array initialisation.
    """
    prev = arr
    for i in range(len(arr.shape)-1):
        curr = np.zeros((prev.shape[0:-1]),dtype='object')
        for index,_ in np.ndenumerate(curr):
            if i==0:
                curr[index] ="\t"*len(curr.shape) + "{" + ",".join([ str(val) for val in prev[index]]) +"}"
            else:
                curr[index] ="\t"*len(curr.shape) + "{\n" + ",\n".join([ str(val) for val in prev[index]]) + "\n" + "\t"*len(curr.shape) + "}"
        prev = curr
    return ",\n".join([i for i in prev])
