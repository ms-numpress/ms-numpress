% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{optimalLinearFixedPoint}
\alias{optimalLinearFixedPoint}
\title{optimalLinearFixedPoint}
\usage{
optimalLinearFixedPoint(data)
}
\arguments{
\item{data}{pointer to array of double to be encoded (need memorycont. repr.)}
}
\value{
the linear fixed point safe to use
}
\description{
Compute the maximal linear fixed point that prevents integer overflow.
}
