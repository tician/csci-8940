
clear all;
more off;
%format long;
output_precision(30);

VERBOSE = 1;

ZERO_FITNESS_LIMIT = 1.0e-5;

TRIAL_LIMIT = 50;
GENERATION_LIMIT = 30;
POPULATION_LIMIT = 20;

NUMBER_GENES = 44;

INERTIA = 0.97;
ACC_COG = 1.5;
ACC_SOC = 2.0;


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


	population.pop = randint(POPULATION_LIMIT,1,[0,(2^(NUMBER_GENES)-1)]);
	population.vel = rand(POPULATION_LIMIT,1);

	%Generate first generation (random)
%	for iter=1:1:POPULATION_LIMIT
%		population(iter) = populator(NUMBER_GENES);
%	end
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
		First_Pop(1,trial,generation) = population(uni_ind(1));
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



	end
	% End of Generation
end
% End of Trial
filename = sprintf("./pso_output_%f_%f_%f_%f_%d_%d_%d",...
	ZERO_FITNESS_LIMIT, INERTIA, ACC_COG, ACC_SOC,...
	TRIAL_LIMIT, POPULATION_LIMIT, GENERATION_LIMIT);
/small_First_Fit  = First_Fit(:,:,:);
small_Second_Fit = Second_Fit(:,:,:);
small_Third_Fit  = Third_Fit(:,:,:);
small_First_Pop  = First_Pop(:,:,:);
small_Second_Pop = Second_Pop(:,:,:);
small_Third_Pop  = Third_Pop(:,:,:);

save( filename,	"small_First_Fit", "small_First_Pop",...
				"small_Second_Fit", "small_Second_Pop",...
				"small_Third_Fit", "small_Third_Pop");

