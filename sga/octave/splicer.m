function [ba,by] = splicer(ma, pa, rate, points, genes)

% Crossover loci (0 ~ 24)
% boost::uniform_int<int> xvr_dist(0,NUMBER_DISEASES-1);

iter=1;
jter=1;
locus = zeros(1,points);

for iter=1:1:points
	if ((rand() * (1/rate)) < 1)
		locus(iter) = uint32( rand() * genes );
	end
end

locus = sort(locus);

ba = ma;
by = pa;

for iter=1:1:points
	if (locus(iter) > 0)
		for jter=locus(iter):1:genes
			temp = ba(jter);
			ba(jter) = by(jter);
			by(jter) = temp;
		end
	end
end

