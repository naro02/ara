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
      s += "%02x" % bs[i+3-n]
    print("    .word 0x%s" % s)



############
## SCRIPT ##
############

if len(sys.argv) == 2:
  vsize = int(sys.argv[1])
else:
  print("Error. Give me one argument: N.")
  print("Vector addition with vectors of size N.")
  sys.exit()

dtype = np.int64

# UPPER_LIMIT = 10000
# LOWER_LIMIT = -10000
UPPER_LIMIT = 10
LOWER_LIMIT = -10


# Input vector
A = np.random.randint(LOWER_LIMIT, UPPER_LIMIT, size=(vsize)).astype(dtype)
C = np.zeros(2 * vsize - 1, dtype=dtype)

# Golden result vector
G = np.zeros(2 * vsize - 1, dtype=dtype)

# Calculate golden result (autocorrelation)
for k in range(-vsize + 1, vsize):
    if k < 0:
        G[k + vsize - 1] = np.sum(A[:vsize + k] * A[-k:])
    else:
        G[k + vsize - 1] = np.sum(A[k:] * A[:vsize - k])

# Create the file
print(".section .data,\"aw\",@progbits")
emit("vsize", np.array(vsize, dtype=np.uint64))
emit("a", A, 'NR_LANES*4')
emit("c", C, 'NR_LANES*4')
emit("g", G, 'NR_LANES*4')