suppressPackageStartupMessages(library(qap))
suppressPackageStartupMessages(library(sna))
suppressPackageStartupMessages(library(argparse))

parser <- ArgumentParser()
parser$add_argument("-wf", "--weightMatFile", required=TRUE, help="weight Matrix File")
parser$add_argument("-df", "--distMatFile", required=TRUE, help="dist Matrix File")
parser$add_argument("-of", "--outputFile", default="", help="output file")
parser$add_argument("-v", "--verbose", action="store_true", default=TRUE, help="Print extra output")
parser$add_argument("-r", "--rep", type="integer", default=100, help="qap param rep")
parser$add_argument("-m", "--miter", type="integer", default=-1, help="qap param miter")
parser$add_argument("-fi", "--fiter", type="double", default=1.1, help="qap param fiter")
parser$add_argument("-ft", "--ft", type="double", default=0.5, help="qap param ft")
parser$add_argument("-maxsteps", "--maxsteps", type="integer", default=50, help="qap param maxsteps")
args <- parser$parse_args()

w <- read.csv(args$weightMatFile, header = FALSE, sep = ",") 
d <- read.csv(args$distMatFile, header = FALSE, sep = ",") 

if (args$miter >= 0) {
  t<-proc.time()
  a <- qap(w, d, rep=args$rep, miter=args$miter, fiter=args$fiter, ft=args$ft, maxsteps=args$maxsteps, verbose=args$verbose)
  proc.time()-t
} else {
  t<-proc.time()
  a <- qap(w, d, rep=args$rep, fiter=args$fiter, ft=args$ft, maxsteps=args$maxsteps, verbose=args$verbose)
  proc.time()-t
}

a <- a - 1

out <- args$outputFile
if (args$outputFile == "") {
  out <- paste(dirname(args$weightMatFile), "/qapresult.txt", sep="")
}
write.table(a, file=out, row.names = FALSE, col.names = FALSE)
