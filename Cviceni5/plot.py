import sys
from matplotlib import pyplot as plt
import numpy as np

inputFileName = "Cviceni5/data.txt"

outputFileName = "vystup.png"


f = open(inputFileName, "r")
dat = []

h = float(f.readline())
n = int(f.readline())

for line in f:
    dat.append(float(line))

f.close()

u = [[0]*n]*n

j = 1
while(j<=n):
    i = 1
    while(i<=n):
        u[i][j] = dat[(j-1)*n + i]
        i = i + 1
    j = j + 1



# generate 2 2d grids for the x & y bounds
y, x = np.meshgrid(np.linspace(0, 1, n), np.linspace(0, 1, n))

z = (1 - x / 2. + x ** 5 + y ** 3) * np.exp(-x ** 2 - y ** 2)
# x and y are bounds, so z should be the value *inside* those bounds.
# Therefore, remove the last value from the z array.
z = z[:-1, :-1]
z_min, z_max = -np.abs(z).max(), np.abs(z).max()

fig, ax = plt.subplots()

c = ax.pcolormesh(x, y, z, cmap='RdBu', vmin=z_min, vmax=z_max)
ax.set_title('pcolormesh')
# set the limits of the plot to the limits of the data
ax.axis([x.min(), x.max(), y.min(), y.max()])
fig.colorbar(c, ax=ax)

plt.show()





plt.figure(figsize=(8,6))

plt.plot(range(0, step), res, "r-", label="HLL")


plt.yscale("log")
plt.ylabel("||rho||")
plt.xlabel("Iter")
plt.legend()

plt.tight_layout()
plt.savefig(outputFileName, dpi=200)

print("Byl vytvořen soubor ", outputFileName)

plt.close()