from matplotlib import pyplot as plt

inputFileName = "NROPR/results.txt"
outputFileName = "NROPR/results.png"

f = open(inputFileName, "r")
res = []

h = []
E = []

for line in f:
    aux = line.split(",")
    h.append(float(aux[0]))
    E.append(float(aux[1]))

f.close()

plt.figure()

plt.plot(h, E, "kx-")
plt.yscale("log")
plt.xscale("log")
plt.ylabel("E")
plt.xlabel("h")

plt.tight_layout()
plt.savefig(outputFileName)

print("Byl vytvořen soubor ", outputFileName)

plt.close()