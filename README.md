MS Numpress
===========

Implementations of two compression schemes for numeric data from mass spectrometers.

The library provides implementations of 3 different algorithms, 
1 designed to compress first order smooth data like retention 
time or M/Z arrays, and 2 for compressing non smooth data with
lower requirements on precision like ion count arrays.

Native implementations and unit test are provided in C++, Java, and C#. We provide Python bindings through [PyMSNumpress](https://pypi.org/project/PyMSNumpress/) which can be installed as follows:

	pip install PyMSNumpress

We provide R bindings through [RMSNumpress](https://CRAN.R-project.org/package=RMSNumpress) which can be installed as follows:
```
install.packages("RMSNumpress")
```

If you use R via Anaconda, it is appropriate to install `RMSNumpress` through **conda** using the **conda-forge** channel. The feedstock can be found [here](https://github.com/conda-forge/r-rmsnumpress-feedstock).

First make sure you have the conda-forge channel
```
conda config --add channels conda-forge
```

Then you can install RMSNumpress via conda
```
conda install r-rmsnumpress
```


### C++ library tests

For C++, move to `src/main/cpp` and compile and run tests (on LINUX) with

	g++ MSNumpress.cpp MSNumpressTest.cpp -o test && ./test

### Java (maven) library tests

Ensure that maven (2.2+) is installed. Then, in this directory, run

	mvn test

### Python library tests

Ensure that Cython and the Python headers are installed on your system. Then
move to `src/main/python` and compile and run tests (on LINUX) with

	python setup.py build_ext --inplace 
	nosetests test_pymsnumpress.py  

### C# library tests

Ensure that a version of Visual Studio is installed on your system. Then open a Visual Studio Cross Tools Command Prompt, 
move to `src\main\csharp` and compile\run tests (on WINDOWS) with

	csc /target:library MSNumpress.cs MSNumpressTest.cs /reference:"C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\PublicAssemblies\Microsoft.VisualStudio.QualityTools.UnitTestFramework.dll"
	MSTest /testcontainer:MSNumpress.dll

NOTE: The example above is for Visual Studio Community 2015 (v14.0). If you use a different version, your path to the unit test reference DLL will be slightly different.

### R library tests

Ensure that `Rcpp` and `devtools` is installed. Then move to `src/main/R/RMSNumpress/` and compile and run tests (on LINUX) with

```
R -e "Rcpp::compileAttributes(); devtools::test()"
```

Numpress Pic
------------
### MS Numpress positive integer compression

Intended for ion count data, this compression simply rounds values 
to the nearest integer, and stores these integers in a truncated 
form which is effective for values relatively close to zero. 


Numpress Slof
-------------
### MS Numpress short logged float compression

Also targeting ion count data, this compression takes the natural
logarithm of values, multiplies by a scaling factor and rounds to 
the nearest integer. For typical ion count dynamic range these values 
fits into two byte integers, so only the two least significant bytes 
of the integer are stored.

The scaling factor can be chosen manually, but the library also contains
a function for retrieving the optimal Slof scaling factor for a given data array.
Since the scaling factor is variable, it is stored as a regular double 
precision float first in the encoding, and automatically parsed during decoding.

Numpress Lin
------------
### MS Numpress linear prediction compression

This compression uses a fixed point representation, achieve by 
multiplication by a scaling factor and rounding to the nearest integer. 
To exploit the assumed linearity of the data, linear prediction is 
then used in the following way. 

The first two values are stored without compression as 4 byte integers.
For each following value a linear prediction is made from the two previous
values:

	Xpred 	= (X(n) - X(n-1)) + X(n)
	Xres 	= Xpred - X(n+1)

The residual `Xres` is then stored, using the same truncated integer 
representation as in Numpress Pic.  

The scaling factor can be chosen manually, but the library also contains
a function for retrieving the optimal Lin scaling factor for a given data array.
Since the scaling factor is variable, it is stored as a regular double 
precision float first in the encoding, and automatically parsed during decoding.

Truncated integer representation 
---------------------------------

This encoding works on a 4 byte integer, by truncating initial zeros or ones.
If the initial (most significant) half byte is 0x0 or 0xf, the number of such 
halfbytes starting from the most significant is stored in a halfbyte. This initial 
count is then followed by the rest of the ints halfbytes, in little-endian order. 
A count halfbyte c of

	0 <= c <= 8 		is interpreted as an initial c 		0x0 halfbytes
	9 <= c <= 15		is interpreted as an initial (c-8) 	0xf halfbytes

Examples:

	int		c		rest
	0 	=> 	0x8
	-1	=>	0xf		0xf
	23	=>	0x6 	0x7	0x1



License 
-------

This code is open source. It is dual licenced under the Apache 2.0 license as
well as the 3-clause BSD licence. See the LICENCE-BSD and the LICENCE-APACHE
file for the licences.

