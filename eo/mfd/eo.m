clear all;
more off;
%format long;
output_precision(30);

TRIAL_LIMIT = 20;
%ITERATION_LIMIT = 50;
ITERATION_LIMIT = 100;
%ITERATION_LIMIT = 30 * NUMBER_VARIABLES;

ZERO_FITNESS_LIMIT = 1.0e-6;
NUMBER_SYMPTOMS = 10;
NUMBER_VARIABLES = 25;
% Possible Diagnoses: 0 ~ 33 554 431 (2^(25)-1) (None to All)


%[qPriorProbability,qManifestationInDisease] = TendencyMatrix10x25;
%qOptimumDiagnoses = ExhaustiveResults10x25;

% Fix tendency values that are less than lower limit
%qManifestationInDisease = tendencyFix(qManifestationInDisease, NUMBER_VARIABLES, NUMBER_SYMPTOMS, ZERO_FITNESS_LIMIT);
%load PreFixedManifestationInDisease;
%qManifestationInDisease

% Only need to calculate prior likelihood once
%qPriorLikelihood = priorLikelihoodSetup(NUMBER_VARIABLES,qPriorProbability);
%load PreComputedPriorLikelihood;
%qPriorLikelihood

%load PreFixedManifestationInDisease;
%load PreComputedPriorLikelihood;

[qPriorLikelihood,qManifestationInDisease] = TendencyMatrix10x25;
qManifestationInDisease = tendencyFix(qManifestationInDisease, NUMBER_VARIABLES, NUMBER_SYMPTOMS, ZERO_FITNESS_LIMIT);

% Cycle through all possible symptom sets except healthy
symptom_set = 1;
for symptom_set=1:1:(2^NUMBER_SYMPTOMS)-1
	printf("Symptom_set: %d\n",symptom_set)

	curr_conf = zeros(TRIAL_LIMIT,ITERATION_LIMIT);
	best_conf = zeros(TRIAL_LIMIT,2);

	% Repeat for some number of trials
	trial = 1;
	for trial=1:1:TRIAL_LIMIT
		printf(" Trial: %d\n",trial)

		%Generate initial configuration (random)
		curr_conf(trial,1) = randint(1,1,[0,(2^(NUMBER_VARIABLES))-1]);
		best_conf(trial,1) = curr_conf(trial,1);

		if bitget(curr_conf(trial,1),26)
			printf(" SHIT!!!! It's init\n")
			return
		end

		% Repeat for some number of iterations
		for iteration=1:1:ITERATION_LIMIT
%			printf("  Iteration: %d\n",iteration)

			clear sor_fit sor_ind uni_fit;
			clear curr_var_fit curr_conf_fit;

			curr_var_fit = zeros(1,NUMBER_VARIABLES);

			% Start fitness evaluations
			curr_var_fit(:) = fit_var(curr_conf(trial,iteration), symptom_set, qPriorLikelihood, qManifestationInDisease, NUMBER_VARIABLES, NUMBER_SYMPTOMS, ZERO_FITNESS_LIMIT);
			% End of fitness evaluations

%			dec2bin(curr_conf(trial,iteration),NUMBER_VARIABLES);
			curr_conf_fit = fit_con(curr_conf(trial,iteration), symptom_set, qPriorLikelihood, qManifestationInDisease, NUMBER_VARIABLES, NUMBER_SYMPTOMS, ZERO_FITNESS_LIMIT);

			% Update best configuration and fitness
			if (curr_conf_fit > best_conf(trial,2))
				best_conf(trial,1) = curr_conf(trial,iteration);
				best_conf(trial,2) = curr_conf_fit;
			end

			if bitget(curr_conf(trial,iteration),26)
				printf(" WHAT THE FUCK!!!!\n")
				return
			end

			% Modify lowest fitness variable if not on last iteration
			if (iteration<ITERATION_LIMIT)
%				[min_fit,min_ind] = min(curr_fit);
%				curr_conf(trial,iteration+1) = bitxor( curr_conf(trial,iteration), bitshift(1,min_ind) );

				uni_fit = unique(curr_var_fit);
				uni_len = length(uni_fit);

				if (uni_len < 2)
					curr_conf(trial,iteration+1) = bitxor( curr_conf(trial,iteration), bitshift(1, randint(1,1,[0,NUMBER_VARIABLES-1]) ) );
%					indi = randint(1,1,[1,NUMBER_VARIABLES]);
%					if bitget(curr_conf(trial,iteration),indi)
%						curr_conf(trial,iteration+1) = bitset(curr_conf(trial,iteration), indi, 0);
%					else
%						curr_conf(trial,iteration+1) = bitset(curr_conf(trial,iteration), indi, 1);
%					end
				else
					indi = ceil(abs(normrnd(0,sqrt(uni_len),1,1)));
					[sor_fit,sor_ind] = sort(curr_var_fit, 'ascend');

					if (indi > uni_len)
						perm_ind = randperm(NUMBER_VARIABLES-uni_len);
						temp_ind = sor_ind;
						for iter=1:1:indi+1
							sor_ind(uni_len+iter) = temp_ind(perm_ind(iter));
						end
					end
					if (sor_ind(indi) > 25)
						printf(" FUCK!!!!\n")
						return
					end

%					if bitget(curr_conf(trial,iteration),sor_ind(indi))
%						curr_conf(trial,iteration+1) = bitset(curr_conf(trial,iteration), sor_ind(indi), 0);
%					else
%						curr_conf(trial,iteration+1) = bitset(curr_conf(trial,iteration), sor_ind(indi), 1);
%					end
					curr_conf(trial,iteration+1) = bitxor( curr_conf(trial,iteration), bitshift(1, sor_ind( indi )-1 ) );

				end


%{
				if (uni_len < 2)
					curr_conf(trial,iteration+1) = bitxor( curr_conf(trial,iteration), bitshift(1, randint(1,1,[1,NUMBER_VARIABLES]) ) );

				else
					[sor_fit,sor_ind] = sort(curr_var_fit, 'ascend');
				
%					curr_conf(trial,iteration+1) = bitxor( curr_conf(trial,iteration), bitshift(1, sor_ind(uint32( (abs(randn())/2) * NUMBER_VARIABLES + 1)) ) );
%					curr_conf(trial,iteration+1) = bitxor( curr_conf(trial,iteration), bitshift(1, sor_ind( ceil(abs(normrnd(0,sqrt(NUMBER_VARIABLES),1,1))) ) ) );
%					curr_conf(trial,iteration+1) = bitxor( curr_conf(trial,iteration), bitshift(1, sor_ind(1)-1 ) );

					curr_conf(trial,iteration+1) = bitxor( curr_conf(trial,iteration), bitshift(1, sor_ind( ceil(abs(normrnd(0,sqrt(uni_len),1,1))) )-1 ) );
				end
%}
			end
			% End of Iteration
		end
		% End of Trial
		dec2bin(best_conf(trial,1),NUMBER_VARIABLES)
		best_conf(trial,2)
	end
	% End of Symptom Set

	filename = sprintf("./eo_mfd_history_%04d_%d.csv", symptom_set, ITERATION_LIMIT);
	csvwrite(filename, curr_conf);
	filename = sprintf("./eo_mfd_best_%04d_%d.csv", symptom_set, ITERATION_LIMIT);
	csvwrite(filename, best_conf);
end

