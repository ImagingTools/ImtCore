import os, fnmatch, sys

path = sys.argv[1]

if os.path.exists('CGenerateRevisionControllerTest.h'):
    os.remove('CGenerateRevisionControllerTest.h')
for entry in os.listdir(path):
    if (os.path.isfile(os.path.join(path, entry)) and fnmatch.fnmatch(entry, "*.acc")):
        with open('CGenerateRevisionControllerTest.h', 'a') as file:
            filename = entry[:-4]
            file.write('const char partitura'+filename+'[] = "'+filename+'";\n')
            file.write('typedef CRevisionControllerPartituraTestBaseRegister<partitura'+filename+'> partitura'+filename+'Class;\n')
            file.write('I_ADD_TEST(partitura'+filename+'Class);\n\n')
