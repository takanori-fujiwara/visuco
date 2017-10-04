library(qap)
library(sna)

dir <- "Work/traceViz/data/output/miniAMR_n256_c1_s1/"
adj <- read.csv(paste(dir, "adj_mat.csv", sep=""), header = FALSE, sep = ",") 
w <- read.csv(paste(dir, "route_weight_mat.csv", sep=""), header = FALSE, sep = ",") 
w_after <- read.csv(paste(dir, "route_weight_mat_qap.csv", sep=""), header = FALSE, sep = ",") 

adj <- as.matrix(adj)

g <- geodist(adj)
d <- as.matrix(g$gdist)

write.table(d, file=paste(dir, "distances.csv", sep=""), sep = ",", row.names = FALSE, col.names = FALSE)
#write.table(w * d, file=paste(dir, "wd_before.csv", sep=""), sep = ",", row.names = FALSE, col.names = FALSE)
#write.table(w_after * d, file=paste(dir, "wd_after.csv", sep=""), sep = ",", row.names = FALSE, col.names = FALSE)

t<-proc.time()
a <- qap(w, d, rep=100, verbose=TRUE)
proc.time()-t

a <- a - 1

write.table(a, file=paste(dir, "qapresult.txt", sep=""), row.names = FALSE, col.names = FALSE)

w.d <- dist(w)
h <- hclust(w.d)
h$order

w <- as.matrix(w)
heatmap(w)
#answers <- list()
#repetitions <- c(1:30)
#for (i in repetitions) {
#  a <- qap(w, d, rep=i)
#  answers <- c(answers, list(attr(a, "obj"))) 
#}
#plot(repetitions, answers)