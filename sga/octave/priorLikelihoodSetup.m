function qPriorLikelihood = priorLikelihoodSetup(genes,qPriorProbability)

iter=0;
jter=0;

for iter=1:1:(2^genes)-1
	qPriorLikelihood(iter) = 1.0;
	for jter=1:1:genes-1
		if bitget(iter, jter)
			qPriorLikelihood(iter) *= ((qPriorProbability(jter))/(1.0-qPriorProbability(jter)));
		end
	end
end
