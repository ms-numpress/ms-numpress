"""
	setup.py
	roest@imsb.biol.ethz.ch
 
	Copyright 2013 Hannes Roest

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
"""

"""
These Python bindings rely on the original C++ code for the actual function
calls. To compile the bindings, you will need Cython and the Python headers
installed on your system.

To build and test, run

$ python setup.py build_ext --inplace
$ nosetests test_pymsnumpress.py  

"""

import os, shutil
from setuptools import setup, Extension
from Cython.Distutils import build_ext

# copy C++ files
try:
    numpress_file = os.path.join( "..", "cpp", "MSNumpress.cpp")
    shutil.copy(numpress_file, ".")
    numpress_file = os.path.join( "..", "cpp", "MSNumpress.hpp")
    shutil.copy(numpress_file, ".")
except IOError:
    pass

ext_modules = [Extension("PyMSNumpress",
                     ["PyMSNumpress.pyx", "MSNumpress.cpp"],
                     language='c++',
                     )]

setup(
    ext_modules = ext_modules,
    cmdclass = {'build_ext': build_ext},

    name="PyMSNumpress",

    version="0.2.3"

)

