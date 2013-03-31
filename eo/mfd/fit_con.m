%{
/*
 * I was really fucking bored and depressed...
 */
%}

function fitness_value = fit_con(diagnosis, symptoms, qPriorLikelihood, qManifestationInDisease, NUMBER_DISEASES, NUMBER_SYMPTOMS)

ZERO_FITNESS_LIMIT = 1.0e-5;

VERBOSE=0;

L1 = 1.0;
L2 = 1.0;
L3 = 1.0;
iter = 1; jter = 1;

if VERBOSE
	diagnosis
end

% Positive Likelihood
for iter=1:1:NUMBER_SYMPTOMS
	if bitget(symptoms, iter)
		temp = 1.0;
		for jter=1:1:NUMBER_DISEASES
			if bitget(diagnosis,jter)
				temp *= (1.0-qManifestationInDisease(iter,jter));
			end
		end
		L1 *= (1.0-temp);
	end
end
if VERBOSE
	L1
end

% Negative Likelihood
for iter=1:1:NUMBER_DISEASES
	if bitget(diagnosis,iter)
		temp = 1.0;
		for jter=1:1:NUMBER_SYMPTOMS
			if bitget(symptoms, jter) == 0
				temp *= (1.0-qManifestationInDisease(jter,iter));
			end
		end
		L2 *= (temp);
	end
end
if VERBOSE
	L2
end

% Prior Likelihood
for iter=1:1:NUMBER_DISEASES
	if bitget(diagnosis,iter)
		L3 *= qPriorLikelihood(iter);
	end
end
if VERBOSE
	L3
end

fitness_value = (L1 * L2 * L3);

