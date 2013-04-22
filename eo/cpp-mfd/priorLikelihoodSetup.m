function qPriorLikelihood = priorLikelihoodSetup(genes,qPriorProbability)

iter=0;

for iter=1:1:genes
	qPriorLikelihood(iter) = ((qPriorProbability(iter))/(1.0-qPriorProbability(iter)));
end
