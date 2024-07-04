import numpy as np
import random
from functools import reduce
import sys

def emit(name, array, alignment='8'):
    print(".global %s" % name)
    print(".balign " + alignment)
    print("%s:" % name)
    bs = array.tobytes()
    for i in range(0, len(bs), 4):
        s = ""
        for n in range(4):
            if i+3-n < len(bs):
                s += "%02x" % bs[i+3-n]
            else:
                s += "00"
        print("    .word 0x%s" % s)

############
## SCRIPT ##
############

if len(sys.argv) == 3:
    rows = int(sys.argv[1])
    cols = int(sys.argv[2])
else:
    print("Error. Give me two arguments: rows and cols.")
    print("2D Auto-correlation with matrices of size rows x cols.")
    sys.exit()

dtype = np.int64

# UPPER_LIMIT = 10000
# LOWER_LIMIT = -10000
UPPER_LIMIT = 10
LOWER_LIMIT = -10

# Input matrix
A = np.random.randint(LOWER_LIMIT, UPPER_LIMIT, size=(rows, cols)).astype(dtype)
C = np.zeros((2 * rows - 1, 2 * cols - 1), dtype=dtype)

# Golden result matrix
G = np.zeros((2 * rows - 1, 2 * cols - 1), dtype=dtype)

# Calculate golden result (2D auto-correlation)
for m in range(-rows + 1, rows):
    for n in range(-cols + 1, cols):
        if m < 0 and n < 0:
            G[m + rows - 1, n + cols - 1] = np.sum(A[:rows + m, :cols + n] * A[-m:, -n:])
        elif m < 0:
            G[m + rows - 1, n + cols - 1] = np.sum(A[:rows + m, n:] * A[-m:, :cols - n])
        elif n < 0:
            G[m + rows - 1, n + cols - 1] = np.sum(A[m:, :cols + n] * A[:rows - m, -n:])
        else:
            G[m + rows - 1, n + cols - 1] = np.sum(A[m:, n:] * A[:rows - m, :cols - n])

# Create the file
print(".section .data,\"aw\",@progbits")
emit("rows", np.array(rows, dtype=np.uint64))
emit("cols", np.array(cols, dtype=np.uint64))
emit("a", A, 'NR_LANES*4')
emit("c", C, 'NR_LANES*4')
emit("g", G, 'NR_LANES*4')
