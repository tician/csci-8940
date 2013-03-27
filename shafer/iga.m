
clear all;
more off;
%format long;
output_precision(30);

VERBOSE = 0;

ZERO_FITNESS_LIMIT = 1.0e-5;

TRIAL_LIMIT = 50;
GENERATION_LIMIT = 30;
POPULATION_LIMIT = 80;

NUMBER_GENES = 44;
TOURNAMENT_SIZE = 4;

%MUTATION_RATE = 0.3*POPULATION_LIMIT;
%CROSSOVER_RATE = 0.06*POPULATION_LIMIT;
MUTATION_RATE = 0.003;
CROSSOVER_RATE = 0.06;
CROSSOVER_POINTS = 2;



%generator_state = rand("state")

% Possible Diagnoses: 1 ~ 33 554 432 (2^(25)) (One to All)
% Returns array of 25 integers
% usage: population = populator(NUMBER_DISEASES)

% Mutate when uint32(rand()*(1/MUTATION_RATE)) = 0
% Returns 1 if mutate, 0 if not
% usage: if mutate(MUTATION_RATE)

% Crossover loci (1 ~ 25)
% returns a location at which to perform crossover
% usage: locus = splicer(NUMBER_DISEASES)





% Population
population = zeros(POPULATION_LIMIT,1);
next_population = zeros(POPULATION_LIMIT,1);



% Store some statistics
%	best_t First[NUMBER_SYMPTOMS];
%	best_t Second[NUMBER_SYMPTOMS];
%	best_t Third[NUMBER_SYMPTOMS];
%	uint32_t EvaluationsToOptimum[(1<<NUMBER_SYMPTOMS)-1][TRIAL_LIMIT];

First_Fit  = zeros(1, TRIAL_LIMIT, GENERATION_LIMIT);
Second_Fit = zeros(1, TRIAL_LIMIT, GENERATION_LIMIT);
Third_Fit  = zeros(1, TRIAL_LIMIT, GENERATION_LIMIT);

First_Pop  = zeros(1, TRIAL_LIMIT, GENERATION_LIMIT);
Second_Pop = zeros(1, TRIAL_LIMIT, GENERATION_LIMIT);
Third_Pop  = zeros(1, TRIAL_LIMIT, GENERATION_LIMIT);

% Repeat for some number of trials
trial = 1;
for trial=1:1:TRIAL_LIMIT
	printf(" Trial: %d\n",trial)

	%Generate first generation (random)
	for iter=1:1:POPULATION_LIMIT
		population(iter) = populator(NUMBER_GENES);
	end
%	population

	% Repeat for some number of generations
	convergence = 0;
	generation = 1;
	for generation=1:1:GENERATION_LIMIT
		if convergence > 9
			printf("   Converged at generation: %d\n", generation-1)
			break;
		end
		if (generation>1)
			population(:) = next_population(:);
		end

		if (VERBOSE > 0)
			dec2bin(population)
		end

		printf("  Generation: %d\n",generation)
%		pause(1)
%		population

%		clear first_f first_g first_i;
%		clear second_f second_g second_i;
%		clear third_f third_g third_i;
		clear uni_fit uni_ind;
%		clear sorted_fitnesses sorted_indices;
		clear sin_fit;
		clear ma pa ba by;
		clear iter jter kter;

		% Cycle through the entire population to calculate fitnesses
		individual = 1;
		sin_fit = zeros(1,POPULATION_LIMIT);

		for individual=1:1:POPULATION_LIMIT
			sin_fit(individual) = fitness(population(individual));
		end
		% End of fitness evaluations

%		[sorted_fitnesses,sorted_indices] = sort(sin_fit, 'descend');
%		First_Fit(1,trial,generation) = sorted_fitnesses(1);
%		First_Pop(1,trial,generation) = population(sorted_indices(1));
%		Second_Fit(1,trial,generation) = sorted_fitnesses(2);
%		Second_Pop(1,trial,generation) = population(sorted_indices(2));
%		Third_Fit(1,trial,generation) = sorted_fitnesses(3);
%		Third_Pop(1,trial,generation) = population(sorted_indices(3));

		[uni_fit, uni_ind] = unique(sin_fit, 'first');

%		[sorted_fitnesses,sorted_indices] = sort(uni_fit, 'descend');
%		[nr,nc] = size(sorted_fitnesses);
%		nc
%		First_Fit(1,trial,generation) = sorted_fitnesses(1);
%		First_Pop(1,trial,generation) = population(uni_ind(sorted_indices(1)));
%		if (nc>1)
%			Second_Fit(1,trial,generation) = sorted_fitnesses(2);
%			Second_Pop(1,trial,generation) = population(uni_ind(sorted_indices(2)));
%		end
%		if (nc>2)
%			Third_Fit(1,trial,generation) = sorted_fitnesses(3);
%			Third_Pop(1,trial,generation) = population(uni_ind(sorted_indices(3)));
%		end


		[nr,nc] = size(uni_fit);

		nc
		First_Fit(1,trial,generation) = uni_fit(nc);
		First_Pop(1,trial,generation) = population(uni_ind(nc));
		if (nc>1)
			Second_Fit(1,trial,generation) = uni_fit(nc-1);
			Second_Pop(1,trial,generation) = population(uni_ind(nc-1));
		end
		if (nc>2)
			Third_Fit(1,trial,generation) = uni_fit(nc-2);
			Third_Pop(1,trial,generation) = population(uni_ind(nc-2));
		end


		First_Fit(1,trial,generation)
		dec2bin(First_Pop(1,trial,generation))
		Second_Fit(1,trial,generation)
		dec2bin(Second_Pop(1,trial,generation))
		Third_Fit(1,trial,generation)
		dec2bin(Third_Pop(1,trial,generation))

		if (generation > 4) && (First_Fit(1,trial,generation) <= First_Fit(1,trial,generation-1))
			convergence += 1;
		else
			convergence = 0;
		end

		% Breed next generation

		for individual=1:1:uint32(POPULATION_LIMIT/2)
			ma=1;pa=1;
			tourney = randperm(POPULATION_LIMIT);
			
			best = -0.01;
			for iter=1:1:TOURNAMENT_SIZE
				if (sin_fit(tourney(iter)) > best)
					best = sin_fit(tourney(iter));
					ma = tourney(iter);
				end
			end
			best = -0.01;
			for iter=TOURNAMENT_SIZE+1:1:TOURNAMENT_SIZE*2
				if (sin_fit(tourney(iter)) > best)
					best = sin_fit(tourney(iter));
					pa = tourney(iter);
				end
			end


			% Crossover
			[ba,by] = splicer(population(ma), population(pa), CROSSOVER_RATE, CROSSOVER_POINTS, NUMBER_GENES);
%			population(ma)
%			population(pa)
%			ba
%			by
			
			next_population(((individual-1)*2)+1) = ba;
			next_population(((individual-1)*2)+2) = by;
		end
		% End of breeding

		% Mutate children
		for iter=1:1:POPULATION_LIMIT
			for jter=1:1:NUMBER_GENES
				if mutagen(MUTATION_RATE)
%					dec2bin(next_population(iter))
%					sleep(1);

					next_population(iter)=bitxor(next_population(iter), 2^jter);

%					if bitget(next_population(iter),jter) > 0
%						bitset(next_population(iter),jter,0);
%					else
%						bitset(next_population(iter),jter,1);
%					end

%					dec2bin(next_population(iter))
%					sleep(1);
				end
			end
		end
		% End of mutation
	end
	% End of Generation
%	sorted_fitnesses(1)
end
% End of Trial
filename = sprintf("./output_%f_%f_%f_%d_%d_%d",...
	ZERO_FITNESS_LIMIT, CROSSOVER_RATE, MUTATION_RATE,...
	POPULATION_LIMIT, GENERATION_LIMIT, CROSSOVER_POINTS);
small_First_Fit  = First_Fit(:,:,:);
small_Second_Fit = Second_Fit(:,:,:);
small_Third_Fit  = Third_Fit(:,:,:);
small_First_Pop  = First_Pop(:,:,:);
small_Second_Pop = Second_Pop(:,:,:);
small_Third_Pop  = Third_Pop(:,:,:);

save( filename,	"small_First_Fit", "small_First_Pop",...
				"small_Second_Fit", "small_Second_Pop",...
				"small_Third_Fit", "small_Third_Pop");

