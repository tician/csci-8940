%{
  Was using bitmask for diagnoses and symptoms because of memory efficiency,
  but 1) octave makes arrays very easy, 2) using integer arrays should allow
  code reuse, 3) the problem is no where near approaching the memory limit of
  my netbook, and 4) I'm not sure how great the processing advantage is for
  using bitwise operations instead of integer arrays (lower need to pull data
  from RAM instead of CPU cache).
%}

clear all;
%format long;
output_precision(30);

NUMBER_SYMPTOMS = 10;
NUMBER_DISEASES = 25;
ZERO_FITNESS_LIMIT = 1.0e-5;
DIFFERENCE_FROM_OPTIMUM = 1.0e-12;

%[qPriorProbability,qManifestationInDisease] = TendencyMatrix10x25;
qOptimumDiagnoses = ExhaustiveResults10x25;

% Fix tendency values that are less than lower limit
%qManifestationInDisease = tendencyFix(qManifestationInDisease, NUMBER_DISEASES, NUMBER_SYMPTOMS, ZERO_FITNESS_LIMIT);
load PreFixedManifestationInDisease;
%qManifestationInDisease

% Only need to calculate prior likelihood once
%qPriorLikelihood = priorLikelihoodSetup(NUMBER_DISEASES,qPriorProbability);
load PreComputedPriorLikelihood;
%qPriorLikelihood

TRIAL_LIMIT = 10;
GENERATION_LIMIT = 50;
POPULATION_LIMIT = 200;

MUTATION_RATE = 0.003;
CROSSOVER_RATE = 0.1;
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

First  = ones((2^NUMBER_SYMPTOMS)-1, TRIAL_LIMIT, 2)*-0.1;
Second = ones((2^NUMBER_SYMPTOMS)-1, TRIAL_LIMIT, 2)*-0.1;
Third  = ones((2^NUMBER_SYMPTOMS)-1, TRIAL_LIMIT, 2)*-0.1;
EvaluationsToOptimum = zeros((2^NUMBER_SYMPTOMS)-1, TRIAL_LIMIT);

% Cycle through all possible symptom sets except healthy
symptom_set = 1;
for symptom_set=1:1:(2^NUMBER_SYMPTOMS)-1
	printf("Symptom_set: %d\n",symptom_set)
	pause(1)

	% Repeat for some number of trials
	trial = 1;
	for trial=1:1:TRIAL_LIMIT
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
			printf("Generation: %d\n",generation)
			pause(1)

			first_f=-0.1; second_f=-0.1; third_f=-0.1;
			first_g=0;   second_g=0;   third_g=0;

			% Cycle through the entire population to calculate fitnesses
			individual = 1;
			sig_fit = zeros(1,POPULATION_LIMIT);
			sin_fit = zeros(1,POPULATION_LIMIT);
			
			for individual=1:1:POPULATION_LIMIT

				sin_fit(individual) = fitness(population(individual,:),symptom_set, qPriorLikelihood, qManifestationInDisease, NUMBER_DISEASES, NUMBER_SYMPTOMS);

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

				if (sin_fit(individual)>third_f)
					if (sin_fit(individual)>second_f)
						if (sin_fit(individual)>first_f)
							third_f = second_f;
							second_f = first_f;
							first_f = sin_fit(individual);

							third_g = second_g;
							second_g = first_g;
							first_g = generation;
						else
							third_f = second_f;
							second_f = sin_fit(individual);

							third_g = second_g;
							second_g = generation;
						end
					else
						third_f = sin_fit(individual);

						third_g = generation;
					end
				end
			end
			% End of fitness evaluations

			first_f
			second_f
			third_f

			% Update First, Second, and Third best fitness values
			if (first_f > Third(symptom_set,trial,1))
				if (first_f > Second(symptom_set,trial,1))
					if (first_f > First(symptom_set,trial,1))
						First(symptom_set,trial,1) = first_f;
						First(symptom_set,trial,2) = first_g;

						if (second_f > Second(symptom_set,trial,1))
							Second(symptom_set,trial,1) = second_f;
							Second(symptom_set,trial,2) = second_g;

							if (third_f > Third(symptom_set,trial,1))
								Third(symptom_set,trial,1) = third_f;
								Third(symptom_set,trial,2) = third_g;
							end
						elseif (second_f > Third(symptom_set,trial,1))
							Third(symptom_set,trial,1) = second_f;
							Third(symptom_set,trial,2) = second_g;
						end
					else
						Second(symptom_set,trial,1) = first_f;
						Second(symptom_set,trial,2) = first_g;
					end
				else
					Third(symptom_set,trial,1) = first_f;
					Third(symptom_set,trial,2) = first_g;
				end
			end
			% End of best fitness updates

			% Breed next generation
			for individual=1:1:uint32(POPULATION_LIMIT/2)
				ma=1;pa=1;

				temp = rand()*sig_fit(POPULATION_LIMIT);
				for iter=POPULATION_LIMIT-1:1:1
					if (temp > sig_fit(iter)) & (temp < sig_fit(iter+1))
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
				for iter=POPULATION_LIMIT-1:1:1
					if (temp > sig_fit(iter)) & (temp < sig_fit(iter+1))
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
	filename = sprintf("./output_%d_%d",symptom_set,uint32(rand()*1000000000));
	small_First = First(symptom_set,:,:);
	small_Second = Second(symptom_set,:,:);
	small_Third = Third(symptom_set,:,:);
	save( filename, "small_First", "small_Second", "small_Third");
end
% End of Symptom Set

filename = sprintf("./output_full_%d",uint32(rand()*1000000000));
save( filename, "First", "Second", "Third");

