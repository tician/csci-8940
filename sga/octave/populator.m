function population = populator(genes)

% Possible Diagnoses: 0 ~ 33 554 431 (2^(25)-1) (None to All)
% boost::uniform_int<int> pop_dist(0,(1<<(NUMBER_DISEASES))-1);

population = zeros(1,genes)

diagnosis = uint32( rand()*(2^(genes)-1) )

for iter=1:1:genes
	if bitget(diagnosis, iter)
		population(iter) = 1;
	end
end

