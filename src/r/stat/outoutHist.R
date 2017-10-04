suppressPackageStartupMessages(library(qap))
suppressPackageStartupMessages(library(sna))
suppressPackageStartupMessages(library(argparse))

parser <- ArgumentParser()
parser$add_argument("-ef", "--edgeFile", required=TRUE, help="edge file")
parser$add_argument("-of", "--outputFile", default="", help="output File")
parser$add_argument("-n", "--numBins", type="integer", default=100, help="number of bins")
parser$add_argument("-xs", "--xStart", type="integer", default=0, help="x range start")
parser$add_argument("-xe", "--xEnd", type="integer", default=1000, help="x range End")
parser$add_argument("-ys", "--yStart", type="integer", default=0, help="y range start")
parser$add_argument("-ye", "--yEnd", type="integer", default=1000, help="y range End")
args <- parser$parse_args()

e <- read.csv(args$edgeFile, header = FALSE, sep = ",")
e.load  <- e$V3
outFile <- args$outputFile
if (args$outputFile == "") {
  outFile <- paste(dirname(args$edgeFile), "/hist.pdf", sep = "")
}

pdf(outFile)
hist(main="Load on Edges", e.load, n = args$numBins, xlim = c(args$xStart,args$xEnd), ylim = c(args$yStart,args$yEnd))
dev.off()
