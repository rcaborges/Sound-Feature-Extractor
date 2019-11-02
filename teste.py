
from subprocess import check_output
out = check_output(["./sfe", "./beira.wav"])

import json
import numpy as np
data = json.loads(out)
fft = data["fft"]
fft = np.array(fft).T
print(fft.shape)

rms = data["rms"]
rms = np.array(rms)
print(rms.shape)

import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
fig = plt.figure()
plt.figure(figsize=(15,3))
ax = plt.gca()
fft = np.where(fft > 0.001, np.log10(fft), -1)
plt.imshow(fft, cmap='jet', interpolation='bicubic', origin='lower', aspect='auto')
x_ticks = ticker.FuncFormatter(lambda x, pos: '{0:g}'.format(x*0.0116))
ax.xaxis.set_major_formatter(x_ticks)
ax.set_xlim([0,fft.shape[1]])
ax.set_ylim([0,400])
plt.tight_layout()
plt.savefig("fft.png")

fig = plt.figure()
plt.figure(figsize=(15,3))
ax = plt.gca()
plt.plot(rms)
ax.set_xlim([0,rms.shape[0]])
plt.tight_layout()
plt.savefig("rms.png")
