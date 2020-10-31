#!/bin/usr/Rscript
library(RMSNumpress)

data = c(100, 101, 102, 103)
data_long = c(100.0,
              200.0,
              300.00005,
              400.00010,
              450.00010,
              455.00010,
              700.00010)
data_slof = c(100.0,
              200.0,
              300.00005,
              400.00010)
fp_slof = 10000
linear_result = (as.raw(as.hexmode(c("40", "f8", "6a", "00", "00", "00", "00", "00", "80", "96", "98", "00", "20", "1d", "9a", "00", "88"))))

test_that("encodeLinear encodes doubles in data", {
  expect_equal( encodeLinear(data, 100000.0), linear_result )
  expect_equal( length(encodeLinear(data, 100000.0)), 17)
  expect_equal( encodeLinear(data, 100000.0)[1], as.raw(as.hexmode("40")))
  
  expect_equal( length(encodeLinear(data_long, 5.0)), 22)
  
  expect_equal( length(encodeLinear(data_long, 500.0)), 25)
  
  expect_equal( length(encodeLinear(data_long, 5e4)), 29)
  
  expect_equal( length(encodeLinear(data_long, 5e5)), 30)
  
  expect_equal( length(encodeLinear(data_long, 5e6)), 31)
  
  # accurate to 3 sign digits
  result = encodeLinear(data_long, 500.0)
  decoded = decodeLinear(result)
  
  expect_equal(decoded[1], 100, tolerance=3)
  expect_equal(decoded[2], 200, tolerance=3)
  expect_equal(decoded[3], 300, tolerance=3)
  expect_equal(decoded[4], 400.00010, tolerance=3)
  expect_equal(decoded[5], 450.00010, tolerance=3)
  expect_equal(decoded[6], 455.00010, tolerance=3)
  expect_equal(decoded[7], 700.00010, tolerance=3)
})

test_that("decodeLinear decode data encoded with encodeLinear", {
  expect_equal( decodeLinear(as.raw(linear_result)), data )
  expect_equal( length(decodeLinear(as.raw(linear_result))), 4 )
  expect_equal( decodeLinear(as.raw(linear_result))[1], 100 )
})

test_that("encodePic encodes ion counts", {
  encode = encodePic(data)
  expect_equal( length(encode), 6 )
  
  result = decodePic(encode)
  expect_equal( length(result), 4 )
  expect_equal( result[1], 100 )
  expect_equal( result, data )
})

test_that("encodeSlof encodes ion counts", {
  encode = encodeSlof(data_slof, fp_slof)
  expect_equal( length(encode), 16 )
  
  result = decodeSlof(encode)
  expect_equal( length(result), 4 )
  
  expect_true( abs(result[1] - 100) < 1)
  expect_true( abs(result[2] - 200) < 1)
  expect_true( abs(result[3] - 300) < 1)
  expect_true( abs(result[4] - 400) < 1)
})

test_that("Compute the maximal linear fixed point that prevents integer overflow", {
  pt = optimalLinearFixedPoint(data)
  expect_equal(pt, 21262214.0)
})

test_that("Compute the maximal slof fixed point that prevents integer overflow", {
  pt = optimalSlofFixedPoint(data)
  expect_equal(pt, 14110.0)
})

test_that("optimal linear fixed point with a desired m/z accuracy", {
  pt = optimalLinearFixedPointMass(data, 0.001)
  expect_equal(pt, 500.0)
  pt = optimalLinearFixedPointMass(data, 1e-10)
  expect_equal(pt, -1)
})