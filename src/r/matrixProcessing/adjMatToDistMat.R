suppressPackageStartupMessages(library(sna))

adjMatCsv = commandArgs(trailingOnly=TRUE)[1]
adj <- read.csv(paste(adjMatCsv, sep=""), header = FALSE, sep = ",") 
adj <- as.matrix(adj)
g <- geodist(adj)
d <- as.matrix(g$gdist)
write.table(d, file=paste(dirname(adjMatCsv), "/dist_mat.csv", sep=""), sep = ",", row.names = FALSE, col.names = FALSE)