clear all;
format long;

NUMBER_SYMPTOMS = 10;
NUMBER_DISEASES = 25;
ZERO_FITNESS_LIMIT = 1e-5;

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

iter, jter;







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
symptom_set = 0;
for symptom_set=1:1:(2^NUMBER_SYMPTOMS)-1

	% Repeat for some number of trials
	trial = 0;
	for trial=1:1:TRIAL_LIMIT-1
		FoundOptimum = 0;
		EvaluationsToOptimum[symptom_set-1][trial] = 0;

		% Repeat for some number of generations
		uint32_t generation;
		for (generation=0; generation<GENERATION_LIMIT; generation++)
			first_f=0.0; second=0.0; third=0.0;
			first_g=0; second_g=0; third_g=0;

			% Cycle through the entire population
			uint32_t individual;
			for (individual=0; individual<POPULATION_LIMIT; individual++)
				if !FoundOptimum
					EvaluationsToOptimum[symptom_set-1][trial]++;
				end
				
				temp = fitness(population[individual],symptom_set);

				if abs(temp-qOptimumDiagnoses(symptom_set-1,0)) < 0.00001
					FoundOptimum=1;
				end

				if (temp>first_f)
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
		% End of generation
	end
	% End of Trial




	stringstream strstr (stringstream::in | stringstream::out);
	strstr.clear();	strstr.str("");
	strstr << "./output_";
	for (iter=0; iter<8; iter++)
	{
		strstr << splicer;
	}
	strstr << ".txt";
	string outname;
	outname = strstr.str();
	ofstream outfile;
	outfile.open( outname );

	if ( !outfile.is_open() )
	{
		cerr << "Unable to open file: " << outname << "\n";
		return 1;
	}
	for (iter=1; iter<(1<<NUMBER_SYMPTOMS); iter++)
	{
		outfile << "symptom_set:\t" << iter << "\t" << First[iter-1] << "\t" << Second[iter-1] << "\t" << Third[iter-1] << "\n";
	}
	outfile.close();

	return 0;
}

