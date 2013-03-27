%{
/*
 * I was really fucking bored and depressed...
 */
%}

function fitness_value = fitness(individual,volume)

VERBOSE=0;

iter = 1; jter = 1;

fitness_value=0;

if VERBOSE
	individual
end

for iter=1:1:3
	for jter=1:1:73
		if individual(1,iter,jter)
			fitness_value += volume(jter,iter);
		end
	end
end

