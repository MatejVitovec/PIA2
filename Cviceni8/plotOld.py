from matplotlib import pyplot as plt
import numpy as np

inputFileName = "Cviceni8/data0.txt"

outputFileName = "Cviceni8/vystup0.png"


f = open(inputFileName, "r")
dat = []

h = float(f.readline())
n = int(f.readline())
m = int(f.readline())

#TODO m ; n

for line in f:
    dat.append(float(line))

f.close()

u = np.reshape(dat, (-1, n))

u = u.transpose()

y, x = np.meshgrid(np.linspace(0, 1, n), np.linspace(0, 1, n))

u_min, u_max = np.abs(u).min(), np.abs(u).max()

fig, ax = plt.subplots()

c = ax.pcolormesh(x, y, u, cmap='RdBu_r', vmin=u_min, vmax=u_max)
ax.set_title('Poisson')
ax.axis([x.min(), x.max(), y.min(), y.max()])
fig.colorbar(c, ax=ax)

plt.tight_layout()
plt.savefig(outputFileName, dpi=200)

print("Byl vytvořen soubor ", outputFileName)

plt.close()