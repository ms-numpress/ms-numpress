# distutils: language = c++
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

from libcpp.vector cimport vector as libcpp_vector
from cython.operator cimport dereference as deref, preincrement as inc, address as address
from MSNumpress cimport encodeLinear as _encodeLinear
from MSNumpress cimport decodeLinear as _decodeLinear
from MSNumpress cimport optimalLinearFixedPoint as _optimalLinearFixedPoint
from MSNumpress cimport optimalLinearFixedPointMass as _optimalLinearFixedPointMass
from MSNumpress cimport encodeSlof as _encodeSlof
from MSNumpress cimport decodeSlof as _decodeSlof
from MSNumpress cimport optimalSlofFixedPoint as _optimalSlofFixedPoint
from MSNumpress cimport encodePic as _encodePic
from MSNumpress cimport decodePic as _decodePic


def optimalLinearFixedPointMass(data, mz):
    dataSize = len(data)
    cdef libcpp_vector[double] c_data = data

    cdef double result = _optimalLinearFixedPointMass( &c_data[0], dataSize, mz)

    return result

def optimalLinearFixedPoint(data):
    dataSize = len(data)
    cdef libcpp_vector[double] c_data = data

    cdef double result = _optimalLinearFixedPoint( &c_data[0], dataSize)

    return result

def optimalSlofFixedPoint(data):
    dataSize = len(data)
    cdef libcpp_vector[double] c_data = data

    cdef double result = _optimalSlofFixedPoint( &c_data[0], dataSize)

    return result

def decodeLinear(data, result):
    cdef libcpp_vector[unsigned char] c_data = data
    cdef libcpp_vector[double] c_result

    _decodeLinear(c_data, c_result)

    cdef libcpp_vector[double].iterator it_result = c_result.begin()
    while it_result != c_result.end():
        result.append( deref(it_result) )
        inc(it_result)

def encodeLinear(data, result, fixedPoint):
    cdef double c_fixedPoint = fixedPoint
    cdef libcpp_vector[double] c_data = data
    cdef libcpp_vector[unsigned char] c_result

    _encodeLinear(c_data, c_result, c_fixedPoint)

    cdef libcpp_vector[unsigned char].iterator it_result = c_result.begin()
    while it_result != c_result.end():
        result.append( deref(it_result) )
        inc(it_result)

def decodeSlof(data, result):
    cdef libcpp_vector[unsigned char] c_data = data
    cdef libcpp_vector[double] c_result

    _decodeSlof(c_data, c_result)

    cdef libcpp_vector[double].iterator it_result = c_result.begin()
    while it_result != c_result.end():
        result.append( deref(it_result) )
        inc(it_result)

def encodeSlof(data, result, fixedPoint):
    cdef double c_fixedPoint = fixedPoint
    cdef libcpp_vector[double] c_data = data
    cdef libcpp_vector[unsigned char] c_result

    _encodeSlof(c_data, c_result, c_fixedPoint)

    cdef libcpp_vector[unsigned char].iterator it_result = c_result.begin()
    while it_result != c_result.end():
        result.append( deref(it_result) )
        inc(it_result)

def decodePic(data, result):
    cdef libcpp_vector[unsigned char] c_data = data
    cdef libcpp_vector[double] c_result

    _decodePic(c_data, c_result)

    cdef libcpp_vector[double].iterator it_result = c_result.begin()
    while it_result != c_result.end():
        result.append( deref(it_result) )
        inc(it_result)

def encodePic(data, result):
    cdef libcpp_vector[double] c_data = data
    cdef libcpp_vector[unsigned char] c_result

    _encodePic(c_data, c_result)

    cdef libcpp_vector[unsigned char].iterator it_result = c_result.begin()
    while it_result != c_result.end():
        result.append( deref(it_result) )
        inc(it_result)

