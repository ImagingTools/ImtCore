import os, fnmatch, sys

pathToClass = sys.argv[1]
pathToPartitura = pathToClass + '/Partitura'
nameClass = ''

if os.path.exists('CGenerateTest.h'):
    os.remove('CGenerateTest.h')

for item in os.listdir(pathToClass):
    if (item.endswith("TestMain.cpp")):
        nameClass = item[:-12]

for entry in os.listdir(pathToPartitura):
    if (os.path.isfile(os.path.join(pathToPartitura, entry)) and fnmatch.fnmatch(entry, "*.acc")):
        with open('CGenerateTest.h', 'a') as file:
            filename = entry[:-4]
            file.write('const char partitura'+filename+'[] = "'+filename+'";\n')
            file.write('typedef '+nameClass+'PartituraTestBaseRegister<partitura'+filename+'> partitura'+filename+'Class;\n')
            file.write('I_ADD_TEST(partitura'+filename+'Class);\n\n')
