import sys
import os

def get_model_tex(fileName):
	fileName += ".mtl"
	f = open(("D:/Users/CYao/Documents/Visual Studio 2013/Projects/RoomObj/Debug/" + fileName), "r")
	for line in f:
		if line[0:6] == "newmtl":
			tex = line[7:-1]
	f.close()
	# if tex[-9:] == " copy.dds":
	# 	tex = tex[:-9] + ".vmat"

	# else:
	tex = tex[:-4] + ".vmat"

	return tex

def remove_target(string):
	return string.replace("target_", "")

def replace_string(mat):
	rep = "\t\tCVMaterialRemap\n\t\t{\n"
	rep += '\t\t\tstring m_sSearchMaterial = "' + mat + ".vmat" + '"\n'
	rep += '\t\t\tstring m_sReplaceMaterial = "' + "materials/imported/" + get_model_tex(mat) + '"\n\t\t},'
	print rep

########################################################################

matList = []
repStr = ""

f = open("material list.txt", "r")
for line in f:
		try:
				matList.append(line[1:-8])
		except:
				pass
f.close()

for mat in matList:
 	replace_string(mat)

