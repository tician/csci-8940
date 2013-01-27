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
qPriorLikelihood = priorLikelihoodSetup(genes,qPriorProbability)



TRIAL_LIMIT = 10;
GENERATION_LIMIT = 50;
POPULATION_LIMIT = 200;

MUTATION_RATE = 0.003;
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

% Cycle through all possible symptom sets except healthy
symptom_set = 1;
for symptom_set=1:1:(2^NUMBER_SYMPTOMS)-1

	% Repeat for some number of trials
	trial = 1;
	for trial=1:1:TRIAL_LIMIT-1
		FoundOptimum = 0;
		EvaluationsToOptimum(symptom_set,trial) = 0;

		% Repeat for some number of generations
		generation = 1;
		for generation=1:1:GENERATION_LIMIT
			first_f=0.0; second=0.0; third=0.0;
			first_g=0; second_g=0; third_g=0;

			% Cycle through the entire population
			individual = 1;
			for individual=1:1:POPULATION_LIMIT
				if !FoundOptimum
					EvaluationsToOptimum(symptom_set,trial)++;
				end
				
				temp = fitness(population(individual,:),symptom_set, qPriorLikelihood, qManifestationsInDisease, NUMBER_DISEASES, NUMBER_SYMPTOMS);

				if abs(temp-qOptimumDiagnoses(symptom_set,1)) < DIFFERENCE_FROM_OPTIMUM
					FoundOptimum=1;
				end

				if temp > first_f
					third_f = second_f;
					second_f = first_f;
					first_f = temp;
					
					third_g = second_g;
					second_g = first_g;
					first_g = generation;
				end
			end
			% End of fitness evaluations
				
			% Update First, Second, and Third best fitness values
			if first_f > First(symptom_set-1,fitness)
				if (second_f > Second[symptom_set-1].fitness)
					if (third_f > Third[symptom_set-1].fitness)
						Third[symptom_set-1].fitness = third_f;
						Third[symptom_set-1].generation = third_g;
					else
						Third[symptom_set-1].fitness = Second[symptom_set-1].fitness;
						Third[symptom_set-1].generation = Second[symptom_set-1].generation;
					end
					Second[symptom_set-1].fitness = second_f;
					Second[symptom_set-1].generation = second_g;
				else
					Second[symptom_set-1].fitness = First[symptom_set-1].fitness;
					Second[symptom_set-1].generation = First[symptom_set-1].generation;
				end
				First[symptom_set-1].fitness = temp;
				First[symptom_set-1].generation = generation;
			end
		end
		% End of Generation
	end
	% End of Trial
end
% End of Symptom Set

filename = sprintf("./output_%d",uint32(rand()*1000000000));
save filename First Second Third;

outfile << "symptom_set:\t" << iter << "\t" << First[iter-1] << "\t" << Second[iter-1] << "\t" << Third[iter-1] << "\n";

