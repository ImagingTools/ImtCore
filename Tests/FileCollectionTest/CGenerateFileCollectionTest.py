import os, fnmatch, sys

path = sys.argv[1]

if os.path.exists('CGenerateFileCollectionTest.h'):
    os.remove('CGenerateFileCollectionTest.h')
for entry in os.listdir(path):
    if (os.path.isfile(os.path.join(path, entry)) and fnmatch.fnmatch(entry, "*.acc")):
        with open('CGenerateFileCollectionTest.h', 'a') as file:
            filename = entry[:-4]
            file.write('const char partitura'+filename+'[] = "'+filename+'";\n')
            file.write('typedef CFileCollectionPartituraTestBaseRegister<partitura'+filename+'> partitura'+filename+'Class;\n')
            file.write('I_ADD_TEST(partitura'+filename+'Class);\n\n')
