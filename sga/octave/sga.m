%{
  Was using bitmask for diagnoses and symptoms because of memory efficiency,
  but 1) octave makes arrays very easy, 2) using integer arrays should allow
  code reuse, 3) the problem is no where near approaching the memory limit of
  my netbook, and 4) I'm not sure how great the processing advantage is for
  using bitwise operations instead of integer arrays (lower need to pull data
  from RAM instead of CPU cache).
%}

clear all;
more off;
%format long;
output_precision(30);

NUMBER_SYMPTOMS = 10;
NUMBER_DISEASES = 25;
ZERO_FITNESS_LIMIT = 1.0e-5;
DIFFERENCE_FROM_OPTIMUM = 1.0e-12;

[qPriorProbability,qManifestationInDisease] = TendencyMatrix10x25;
qOptimumDiagnoses = ExhaustiveResults10x25;

% Fix tendency values that are less than lower limit
qManifestationInDisease = tendencyFix(qManifestationInDisease, NUMBER_DISEASES, NUMBER_SYMPTOMS, ZERO_FITNESS_LIMIT);
%load PreFixedManifestationInDisease;
%qManifestationInDisease

% Only need to calculate prior likelihood once
qPriorLikelihood = priorLikelihoodSetup(NUMBER_DISEASES,qPriorProbability);
%load PreComputedPriorLikelihood;
%qPriorLikelihood

TRIAL_LIMIT = 10;
GENERATION_LIMIT = 30;
POPULATION_LIMIT = 100;

MUTATION_RATE = 0.03;
CROSSOVER_RATE = 0.6;
CROSSOVER_POINTS = 1;



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
population = zeros(POPULATION_LIMIT,NUMBER_DISEASES);



% Store some statistics
%	best_t First[NUMBER_SYMPTOMS];
%	best_t Second[NUMBER_SYMPTOMS];
%	best_t Third[NUMBER_SYMPTOMS];
%	uint32_t EvaluationsToOptimum[(1<<NUMBER_SYMPTOMS)-1][TRIAL_LIMIT];

First_Fit  = zeros((2^NUMBER_SYMPTOMS)-1, TRIAL_LIMIT, GENERATION_LIMIT);
Second_Fit = zeros((2^NUMBER_SYMPTOMS)-1, TRIAL_LIMIT, GENERATION_LIMIT);
Third_Fit  = zeros((2^NUMBER_SYMPTOMS)-1, TRIAL_LIMIT, GENERATION_LIMIT);

First_Pop  = zeros((2^NUMBER_SYMPTOMS)-1,TRIAL_LIMIT, GENERATION_LIMIT, NUMBER_DISEASES);
Second_Pop = zeros((2^NUMBER_SYMPTOMS)-1,TRIAL_LIMIT, GENERATION_LIMIT, NUMBER_DISEASES);
Third_Pop  = zeros((2^NUMBER_SYMPTOMS)-1,TRIAL_LIMIT, GENERATION_LIMIT, NUMBER_DISEASES);

EvaluationsToOptimum = zeros((2^NUMBER_SYMPTOMS)-1, TRIAL_LIMIT);

% Cycle through all possible symptom sets except healthy
symptom_set = 1;
for symptom_set=1:1:(2^NUMBER_SYMPTOMS)-1
	printf("Symptom_set: %d\n",symptom_set)
%	pause(1)

	% Repeat for some number of trials
	trial = 1;
	for trial=1:1:TRIAL_LIMIT
		printf(" Trial: %d\n",trial)

		%Generate first generation (random)
		for iter=1:1:POPULATION_LIMIT
			population(iter,:) = populator(NUMBER_DISEASES);
		end
%		population

		FoundOptimum = 0;
		EvaluationsToOptimum(symptom_set,trial) = 1;

		% Repeat for some number of generations
		generation = 1;
		for generation=1:1:GENERATION_LIMIT
			printf("  Generation: %d\n",generation)
%			pause(1)
			population

%			clear first_f first_g first_i;
%			clear second_f second_g second_i;
%			clear third_f third_g third_i;
			clear sorted_fitnesses sorted_indices;
			clear sin_fit sig_fit;
			clear ma pa ba by;
			clear iter jter kter;

%			first_f=-0.1; second_f=-0.1; third_f=-0.1;
%			first_g=0;   second_g=0;   third_g=0;
%			first_i=zeros(1,NUMBER_DISEASES);
%			second_i=zeros(1,NUMBER_DISEASES);
%			third_i=zeros(1,NUMBER_DISEASES);

			% Cycle through the entire population to calculate fitnesses
			individual = 1;
			sig_fit = zeros(1,POPULATION_LIMIT);
			sin_fit = zeros(1,POPULATION_LIMIT);
			
			for individual=1:1:POPULATION_LIMIT
				if individual > 1
					if population(individual,:)==population(individual-1,:)
						sin_fit(individual) = sin_fit(individual-1);
					else
						sin_fit(individual) = fitness(population(individual,:),symptom_set, qPriorLikelihood, qManifestationInDisease, NUMBER_DISEASES, NUMBER_SYMPTOMS);
					end
				else
					sin_fit(individual) = fitness(population(individual,:),symptom_set, qPriorLikelihood, qManifestationInDisease, NUMBER_DISEASES, NUMBER_SYMPTOMS);
				end

				% Update Sigma Fitness array for roulette wheel selection
				if individual > 1
					sig_fit(individual) = sig_fit(individual-1) + sin_fit(individual);
				else
					sig_fit(individual) = sin_fit(individual);
				end

				if !FoundOptimum
					EvaluationsToOptimum(symptom_set,trial)++;
					if abs(sin_fit(individual)-qOptimumDiagnoses(symptom_set,1)) < DIFFERENCE_FROM_OPTIMUM
						FoundOptimum=1;
					end
				end
			end
			% End of fitness evaluations

%{
				if ( (sin_fit(individual)>third_f) )%> ZERO_FITNESS_LIMIT )
					if ( (sin_fit(individual)>second_f) )%> ZERO_FITNESS_LIMIT )
						if ( (sin_fit(individual)>first_f) )%> ZERO_FITNESS_LIMIT )
							third_f = second_f;
							second_f = first_f;
							first_f = sin_fit(individual);

							third_g = second_g;
							second_g = first_g;
							first_g = generation;

							third_i = second_i;
							second_i = first_i;
							first_i = population(individual,:);
						elseif ( abs(sin_fit(individual)-first_f) > ZERO_FITNESS_LIMIT )
							third_f = second_f;
							second_f = sin_fit(individual);

							third_g = second_g;
							second_g = generation;

							third_i = second_i;
							second_i = population(individual,:);
						end
					elseif ( abs(sin_fit(individual)-second_f) > ZERO_FITNESS_LIMIT )
						third_f = sin_fit(individual);

						third_g = generation;

						third_i = population(individual,:);
					end
				end
			end
			% End of fitness evaluations

			first_f
			first_i
			second_f
			second_i
			third_f
			third_i

%			sin_fit
%			sig_fit

			% Update First, Second, and Third best fitness values
			if ( (first_f > Third(symptom_set,trial,1)) )%> ZERO_FITNESS_LIMIT )
				if ( (first_f > Second(symptom_set,trial,1)) )%> ZERO_FITNESS_LIMIT )
					if ( (first_f > First(symptom_set,trial,1)) )%> ZERO_FITNESS_LIMIT )
						First(symptom_set,trial,1) = first_f;
						First(symptom_set,trial,2) = first_g;
						First_Pop(symptom_set,trial,:) = first_i;

						if ( (second_f > First(symptom_set,trial,1)) )%> ZERO_FITNESS_LIMIT )
							Second(symptom_set,trial,1) = second_f;
							Second(symptom_set,trial,2) = second_g;
							Second_Pop(symptom_set,trial,:) = second_i;

							if ( (third_f > Third(symptom_set,trial,1)) )%> ZERO_FITNESS_LIMIT )
								Third(symptom_set,trial,1) = third_f;
								Third(symptom_set,trial,2) = third_g;
								Third_Pop(symptom_set,trial,:) = third_i;
							end
						elseif ( (second_f > Third(symptom_set,trial,1)) )%> ZERO_FITNESS_LIMIT )
							Third(symptom_set,trial,1) = second_f;
							Third(symptom_set,trial,2) = second_g;
							Third_Pop(symptom_set,trial,:) = second_i;
						end
					else
						Second(symptom_set,trial,1) = first_f;
						Second(symptom_set,trial,2) = first_g;
						Second_Pop(symptom_set,trial,:) = first_i;
					end
				else
					Third(symptom_set,trial,1) = first_f;
					Third(symptom_set,trial,2) = first_g;
					Third_Pop(symptom_set,trial,:) = first_i;
				end
			end
			% End of best fitness updates
%}
			[sorted_fitnesses,sorted_indices] = sort(sin_fit, 'descend');
			First_Fit(symptom_set,trial,generation) = sorted_fitnesses(1);
			First_Pop(symptom_set,trial,generation,:) = population(sorted_indices(1),:);
			Second_Fit(symptom_set,trial,generation) = sorted_fitnesses(2);
			Second_Pop(symptom_set,trial,generation,:) = population(sorted_indices(2),:);
			Third_Fit(symptom_set,trial,generation) = sorted_fitnesses(3);
			Third_Pop(symptom_set,trial,generation,:) = population(sorted_indices(3),:);

			sorted_fitnesses(1)
			population(sorted_indices(1),:)
			sorted_fitnesses(2)
			population(sorted_indices(2),:)
			sorted_fitnesses(3)
			population(sorted_indices(3),:)

			% Breed next generation
			for individual=1:1:uint32(POPULATION_LIMIT/2)
				ma=1;pa=1;

				temp = rand()*sig_fit(POPULATION_LIMIT);
				for iter=1:1:POPULATION_LIMIT
					if (temp <= sig_fit(iter))
						ma = iter;
						break;
					end
				end
					
%{
				% Bisecting search for parent
				seeker_left = 1;
				seeker_right = POPULATION_LIMIT;
				seeker = uint32((seeker_right+seeker_left)/2);
				% Absolute worst case should be POP_LIMIT/2
				for iter=1:1:POPULATION_LIMIT
					if (temp <= sig_fit(seeker))
						if (seeker>1)
							if (temp > sig_fit(seeker-1))
								ma = seeker;
								break;
							else
								seeker_right = seeker;
								seeker = uint32((seeker_right+seeker_left)/2);
							end
						else
							ma = seeker;
							break;
						end
					else
						if (seeker < POPULATION_LIMIT)
							if (temp <= sig_fit(seeker+1))
								ma = seeker+1;
								break;
							else
								seeker_left = seeker;
								seeker = uint32((seeker_right+seeker_left)/2);
							end
						else
							ma = seeker;
							break;
						end
					end
				end
%}
				temp = rand()*sig_fit(POPULATION_LIMIT);
				for iter=1:1:POPULATION_LIMIT
					if (temp <= sig_fit(iter))
						pa = iter;
						break;
					end
				end
%{
				% Bisecting search for parent
				seeker_left = 1;
				seeker_right = POPULATION_LIMIT;
				seeker = uint32((seeker_right+seeker_left)/2);
				% Absolute worst case should be POP_LIMIT/2
				for iter=1:1:POPULATION_LIMIT
					if (temp <= sig_fit(seeker))
						if (seeker>1)
							if (temp > sig_fit(seeker-1))
								pa = seeker;
								break;
							else
								seeker_right = seeker;
								seeker = uint32((seeker_right+seeker_left)/2);
							end
						else
							pa = seeker;
							break;
						end
					else
						if (seeker < POPULATION_LIMIT)
							if (temp <= sig_fit(seeker+1))
								pa = seeker+1;
								break;
							else
								seeker_left = seeker;
								seeker = uint32((seeker_right+seeker_left)/2);
							end
						else
							pa = seeker;
							break;
						end
					end
				end
%}
				% Crossover
				[ba,by] = splicer(population(ma,:), population(pa,:), CROSSOVER_RATE, CROSSOVER_POINTS, NUMBER_DISEASES);
%				population(ma,:)
%				population(pa,:)
%				ba
%				by
				
				population(((individual-1)*2)+1,:) = ba;
				population(((individual-1)*2)+2,:) = by;
			end
			% End of breeding

			% Mutate children
			for iter=1:1:POPULATION_LIMIT
				for jter=1:1:NUMBER_DISEASES
					if mutagen(MUTATION_RATE)
						if population(iter,jter) > 0
							population(iter,jter) = 0;
						else
							population(iter,jter) = 1;
						end
					end
				end
			end
			% End of mutation
		end
		% End of Generation
	end
	% End of Trial
	filename = sprintf("./output_%f_%f_%f_%d_%d_%d_%d_%d",...
		ZERO_FITNESS_LIMIT, CROSSOVER_RATE, MUTATION_RATE, POPULATION_LIMIT,...
		GENERATION_LIMIT, CROSSOVER_POINTS, symptom_set, uint32(rand()*1000000000));
	small_First_Fit  = First_Fit(symptom_set,:,:);
	small_Second_Fit = Second_Fit(symptom_set,:,:);
	small_Third_Fit  = Third_Fit(symptom_set,:,:);
	small_First_Pop  = First_Pop(symptom_set,:,:,:);
	small_Second_Pop = Second_Pop(symptom_set,:,:,:);
	small_Third_Pop  = Third_Pop(symptom_set,:,:,:);
	
	save( filename,	"small_First_Fit", "small_First_Pop",...
					"small_Second_Fit", "small_Second_Pop",...
					"small_Third_Fit", "small_Third_Pop");
end
% End of Symptom Set

filename = sprintf("./output_full_%f_%f_%f_%d_%d_%d_%d",...
	ZERO_FITNESS_LIMIT, CROSSOVER_RATE, MUTATION_RATE, POPULATION_LIMIT,...
	GENERATION_LIMIT, CROSSOVER_POINTS, uint32(rand()*1000000000));
save( filename, "First", "Second", "Third");

