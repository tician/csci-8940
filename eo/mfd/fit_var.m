
function fitness_value = fit_var(diagnosis, symptoms, qPriorLikelihood, qManifestationInDisease, NUMBER_DISEASES, NUMBER_SYMPTOMS)

ZERO_FITNESS_LIMIT = 1.0e-6;

iter = 1; jter = 1;

fitness_value = ones(1,NUMBER_DISEASES);

for iter=1:1:NUMBER_DISEASES
	L1 = 1.0;
	L2 = 1.0;
	L3 = 1.0;

	if bitget(diagnosis, iter)

		% Positive Likelihood (disease causes symptoms we do have)
		temp = 1.0;
		for jter=1:1:NUMBER_SYMPTOMS
			if bitget(symptoms, jter) > 0
				temp *= (1.0-qManifestationInDisease(jter,iter));
			end
		end
		L1 *= (1.0-temp);

		% Negative Likelihood (disease causes symptoms we do not have)
		temp = 1.0;
		for jter=1:1:NUMBER_SYMPTOMS
			if bitget(symptoms, jter) == 0
				temp *= (1.0-qManifestationInDisease(jter,iter));
			end
		end
		L2 *= (temp);

		% Prior Likelihood
		L3 *= qPriorLikelihood(iter);

	end

	fitness_value(iter) = L1 * L2 * L3;
end
