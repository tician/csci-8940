%{
/*
 * I was really fucking bored and depressed...
 */
%}

function fitness_value = fitness(diagnosis, symptoms, qPriorLikelihood, qManifestationsInDisease, NUMBER_DISEASES, NUMBER_SYMPTOMS)


L1 = 1.0;
L2 = 1.0;
iter = 0; jter = 0;

% Positive Likelihood
for iter=1:1:NUMBER_SYMPTOMS
	if bitget(symptoms, iter)
		temp = 1.0;
		for jter=1:1:NUMBER_DISEASES
			if bitget(diagnosis, jter)
				temp = temp * (1.0-qManifestationInDisease(iter,jter));
			end
			L1 = L1 * (1.0-temp);
		end
	end
end

% Negative Likelihood
for iter=1:1:NUMBER_DISEASES
	if bitget(diagnosis, iter)
		temp = 1.0;
		for jter=1:1:NUMBER_SYMPTOMS
			if bitset(symptoms, jter) == 0
				temp = temp * (1.0-qManifestationInDisease(jter,iter));
			end
			L2 = L2 * (temp);
		end
	end
end

fitness_value = (L1 * L2 * qPriorLikelihood(diagnosis));

