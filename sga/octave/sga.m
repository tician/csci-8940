% Was using bitmask for diagnoses and symptoms because of memory efficiency,
%  but 1) octave makes arrays very easy, 2) using integer arrays should allow
%  code reuse, 3) the problem is no where near approaching the memory limit of
%  my netbook, and 4) I'm not sure how great the processing advantage is for
%  using bitwise operations instead of integer arrays (lower need to pull data
%  from RAM instead of CPU cache).

clear all;
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

% Only need to calculate prior likelihood once
qPriorLikelihood = priorLikelihoodSetup(NUMBER_DISEASES,qPriorProbability);


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


%Generate first generation (random)
for iter=1:1:POPULATION_LIMIT
	population(iter,:) = populator(NUMBER_DISEASES);
end

% Store some statistics
%	best_t First[NUMBER_SYMPTOMS];
%	best_t Second[NUMBER_SYMPTOMS];
%	best_t Third[NUMBER_SYMPTOMS];
%	uint32_t EvaluationsToOptimum[(1<<NUMBER_SYMPTOMS)-1][TRIAL_LIMIT];

First  = zeros((2^NUMBER_SYMPTOMS)-1, TRIAL_LIMIT, 2);
Second = zeros((2^NUMBER_SYMPTOMS)-1, TRIAL_LIMIT, 2);
Third  = zeros((2^NUMBER_SYMPTOMS)-1, TRIAL_LIMIT, 2);
EvaluationsToOptimum = zeros((2^NUMBER_SYMPTOMS)-1, TRIAL_LIMIT);

% Cycle through all possible symptom sets except healthy
symptom_set = 1;
for symptom_set=1:1:(2^NUMBER_SYMPTOMS)-1
	printf("Symptom_set:%d\n",symptom_set);

	% Repeat for some number of trials
	trial = 1;
	for trial=1:1:TRIAL_LIMIT
		FoundOptimum = 0;
		EvaluationsToOptimum(symptom_set,trial) = 1;

		% Repeat for some number of generations
		generation = 1;
		for generation=1:1:GENERATION_LIMIT
			printf("Generation:%d\n",generation);

			first_f=0.0; second_f=0.0; third_f=0.0;
			first_g=0;   second_g=0;   third_g=0;

			% Cycle through the entire population to calculate fitnesses
			individual = 1;
			sig_fit = zeros(1,POPULATION_LIMIT);
			sin_fit = zeros(1,POPULATION_LIMIT);
			
			for individual=1:1:POPULATION_LIMIT

				sin_fit(individual) = fitness(population(individual,:),symptom_set, qPriorLikelihood, qManifestationInDisease, NUMBER_DISEASES, NUMBER_SYMPTOMS);

				% Update Sigma Fitness array for roulette wheel selection
				if individual > 1
					sig_fit(individual) = sin_fit(individual-1) + sin_fit(individual);
				else
					sig_fit(individual) = sin_fit(individual);
				end

				if !FoundOptimum
					EvaluationsToOptimum(symptom_set,trial)++;
					if abs(sin_fit(individual)-qOptimumDiagnoses(symptom_set,1)) < DIFFERENCE_FROM_OPTIMUM
						FoundOptimum=1;
					end
				end

				if sin_fit(individual) > first_f
					third_f = second_f;
					second_f = first_f;
					first_f = sin_fit(individual);
					
					third_g = second_g;
					second_g = first_g;
					first_g = generation;
				end
			end
			% End of fitness evaluations

			printf("\tFirst:%f\tSecond:%f\tThird:%f\n", first_f, second_f, third_f);

			% Update First, Second, and Third best fitness values
			if first_f > First(symptom_set,trial,1)
				if second_f > Second(symptom_set,trial,1)
					if third_f > Third(symptom_set,trial,1)
						Third(symptom_set,trial,1) = third_f;
						Third(symptom_set,trial,2) = third_g;
					else
						Third(symptom_set,trial,1) = Second(symptom_set,trial,1);
						Third(symptom_set,trial,2) = Second(symptom_set,trial,2);
					end
					Second(symptom_set,trial,1) = second_f;
					Second(symptom_set,trial,2) = second_g;
				else
					Second(symptom_set,trial,1) = First(symptom_set,trial,1);
					Second(symptom_set,trial,2) = First(symptom_set,trial,2);
				end
				First(symptom_set,trial,1) = sin_fit(individual);
				First(symptom_set,trial,2) = generation;
			end
			% End of best fitness updates

			% Breed next generation
			for individual=1:1:uint32(POPULATION_LIMIT/2)

				temp = rand()*sig_fit(POPULATION_LIMIT);

				% Bisecting search for parent
				seeker = uint32(POPULATION_LIMIT/2);
				for iter=1:1:POPULATION_LIMIT
					if temp <= sig_fit(seeker)
						if temp > sig_fit(seeker-1)
							ma = seeker;
							break;
						else
							seeker = uint32(seeker/2);
						end
					else
						if temp <= sig_fit(seeker+1)
							ma = seeker+1;
							break;
						else
							seeker = uint32(seeker + (seeker/2));
						end
					end
				end

				temp = rand()*sig_fit(POPULATION_LIMIT);

				% Bisecting search for parent
				seeker = uint32(POPULATION_LIMIT/2);
				for iter=1:1:POPULATION_LIMIT
					if temp <= sig_fit(seeker)
						if temp > sig_fit(seeker-1)
							pa = seeker;
							break;
						else
							seeker = uint32(seeker/2);
						end
					else
						if temp <= sig_fit(seeker+1)
							pa = seeker+1;
							break;
						else
							seeker = uint32(seeker + (seeker/2));
						end
					end
				end

				% Crossover
				[ba,by] = splicer(ma,pa,CROSSOVER_RATE,CROSSOVER_POINTS,NUMBER_DISEASES);
				
				population(((iter-1)*2)+1,:) = ba;
				population(((iter-1)*2)+2,:) = by;
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
end
% End of Symptom Set


filename = sprintf("./output_%d",uint32(rand()*1000000000))
save filename First Second Third;


