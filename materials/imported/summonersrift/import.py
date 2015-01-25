import sys
import os

address = "PSD"
exclude = "Exclude"
allFiles = []
noAlpha = []
base = open("base_vmat.vmat").read()
baseNoAlpha = open("no_alpha.vmat").read()

for item in os.listdir(address):
	allFiles.append(item)

for item in os.listdir(exclude):
	noAlpha.append(item)

for item in allFiles:
	fileName = item.split(".")[0]
	f = open(fileName + ".vmat", "w")
	if item in noAlpha:
		f.write(baseNoAlpha.replace("<insert_texture>", fileName + ".psd"))
	else:
		f.write(base.replace("<insert_texture>", fileName + ".psd"))
	f.close	

print "done"
