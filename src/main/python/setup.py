import os, shutil
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

# copy C++ files
numpress_file = os.path.join( "..", "cpp", "MSNumpress.cpp")
shutil.copy(numpress_file, ".")
numpress_file = os.path.join( "..", "cpp", "MSNumpress.hpp")
shutil.copy(numpress_file, ".")

ext_modules = [Extension("PyMSNumpress",
                     ["PyMSNumpress.pyx", "MSNumpress.cpp"],
                     language='c++',
                     )]

setup(
    ext_modules = ext_modules,
    cmdclass = {'build_ext': build_ext},
)

